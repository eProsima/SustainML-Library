# Copyright 2024 SustainML Consortium
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""SustainML Backend Node Implementation."""

from flask import Flask, request, jsonify
import json
import os
import re
import requests
import signal
import sustainml_swig
import sys
import threading
import time
from orchestrator_node import orchestrator_node, utils
from werkzeug.serving import make_server

running = True
orchestrator = orchestrator_node.Orchestrator()
server = Flask(__name__)
server_ip_address = '127.0.0.1'
server_port = 5001


def _hf_headers(token: str | None):
    if token:
        return {"Authorization": f"Bearer {token}"}
    return {}


def _table_row_from_fact(f: dict) -> dict:
    langs = f.get("languages") or []
    lang_str = ", ".join(langs) if isinstance(langs, list) else str(langs or "")

    arch = f.get("architectures") or []
    if isinstance(arch, list):
        arch_str = ", ".join([str(x) for x in arch if x])
    else:
        arch_str = str(arch or "")

    return {
        "model_id": f.get("model_id") or "",
        "license": f.get("license") or "",
        "model_family": f.get("model_family") or "",
        "architectures": arch_str,
        "languages": lang_str,
    }


def _fetch_hf_repo_json(repo_id: str, filename: str, token: str | None):
    # raw file endpoint; try main then master
    urls = [
        f"https://huggingface.co/{repo_id}/raw/main/{filename}",
        f"https://huggingface.co/{repo_id}/raw/master/{filename}",
    ]
    for url in urls:
        r = requests.get(url, headers=_hf_headers(token), timeout=20)
        if r.status_code == 200:
            try:
                return r.json()
            except Exception:
                return None
    return None


def _fetch_hf_model_info(model_id: str, token: str | None):
    url = f"https://huggingface.co/api/models/{model_id}"
    params = [("expand[]", "cardData"), ("expand[]", "config")]
    r = requests.get(url, headers=_hf_headers(token), params=params, timeout=45)
    return r.status_code, (r.json() if r.headers.get("content-type","").startswith("application/json") else None)


def _collect_all_tags(info: dict) -> list[str]:
    tags = []
    t0 = info.get("tags") or []
    if isinstance(t0, list):
        tags.extend([str(x) for x in t0])

    cd = info.get("cardData") or {}
    if isinstance(cd, dict):
        # Sometimes tags-like info is embedded here
        t1 = cd.get("tags") or []
        if isinstance(t1, list):
            tags.extend([str(x) for x in t1])

        # Also consider common “notes-like” structured fields as pseudo-tags
        for k in ("base_model", "quantization", "library_name", "pipeline_tag"):
            v = cd.get(k)
            if isinstance(v, str) and v.strip():
                tags.append(f"{k}:{v.strip()}")

    # Dedupe preserving order
    seen = set()
    out = []
    for x in tags:
        if x not in seen:
            seen.add(x)
            out.append(x)
    return out


def _model_facts(model_obj: dict, token: str | None) -> dict:
    info = model_obj.get("info") or {}
    mid = model_obj.get("model_id") or info.get("modelId") or info.get("id") or ""

    card = info.get("cardData") or {}
    cfg = info.get("config") or {}
    tags = _collect_all_tags(info)

    if not tags:
        try:
            _, j = _fetch_hf_model_info(mid, token)
            if isinstance(j, dict):
                tags = j.get("tags") or []
        except Exception:
            tags = []

    # License
    license_val = card.get("license")
    if not license_val:
        for t in tags:
            if isinstance(t, str) and t.startswith("license:"):
                license_val = t.split(":", 1)[1]
                break

    arch_list = []
    raw_cfg = _fetch_hf_repo_json(mid, "config.json", token)
    if isinstance(raw_cfg, dict):
        arch_list = raw_cfg.get("architectures") or []
    arch_list = arch_list[:3] if isinstance(arch_list, list) else []

    # Languages
    lang = card.get("language")
    if isinstance(lang, str):
        langs = [lang]
    elif isinstance(lang, list):
        langs = lang[:50]
    else:
        langs = []

    return {
        "model_id": mid,
        "license": license_val or "unknown",
        "architectures": arch_list,
        "model_family": (cfg.get("model_type") or "").strip(),
        "languages": langs
    }


def _build_compare_prompt(facts: list[dict]) -> str:
    return f"""
You are generating a comparison report for a UI.
You MUST return STRICT JSON only. No markdown, no extra text.

Rules:
- Use ONLY the provided Facts.
- Do NOT invent characteristics.
- Be comparative.
- AVOID generic advice.

Return STRICT JSON with this schema:
{{
  "comparison_summary": "string",
  "model_cards": [
    {{
      "model_id": "string",
      "purpose": "string",
      "best_for": ["string"],
      "watch_out_for": ["string"]
    }}
  ],
  "comparison_table": [
    {{
      "model_id": "string",
      "license": "string",
      "model_family": "string",
      "architectures": "string",
      "languages": "string"
    }}
  ]
}}

comparison_summary rules (MANDATORY):
- Write AT LEAST 2 indented paragraphs.
- Total length: at least 8 sentences.
- Mention EVERY model_id at least once.
- Do not rely on unknown characteristics.
- Paragraph 1: big picture + grouping; compare them using facts from Hugging Face.
- Paragraph 2: trade-offs + recommendations + different purposes + risks using facts.

Facts:
{json.dumps(facts, ensure_ascii=False, indent=2)}
""".strip()


def _ollama_chat(prompt: str, model: str = "llama3", timeout: int = 450) -> str:
    url = "http://localhost:11434/api/chat"
    payload = {
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "stream": False,
        "format": "json",
        "options": {"temperature": 0}
    }
    r = requests.post(url, json=payload, timeout=timeout)
    r.raise_for_status()
    j = r.json()
    return ((j.get("message") or {}).get("content") or "").strip()


def _sanitize_json_text(s: str) -> str:
    # Remove ASCII control chars except \n \r \t
    return re.sub(r"[\x00-\x08\x0b\x0c\x0e-\x1f]", "", s)


# Flask server default route
@server.route('/')
def hello_world():
    return jsonify({'message': 'Hello world! Use "/terminate" route to stop Back-end node.'}), 200


# Send user input data to orchestrator
@server.route('/user_input', methods=['POST'])
def user_input():
    global hf_token

    data = request.json

    # 1) Read the model family coming from QML ("Transformers" by default).
    #    QML sends this as the field named "type" in engine.launch_task(..., type)
    incoming_type_top = data.get('type')
    incoming_extra = data.get('extra_data') or {}
    incoming_type_extra = incoming_extra.get('type')
    incoming_model_family = incoming_extra.get('model_family')

    # Resolve final model_family: prefer explicit model_family, then type, then default
    model_family = (
        incoming_model_family
        or incoming_type_top
        or incoming_type_extra
        or 'Transformers'
    )

    # Make sure extra_data exists
    if 'extra_data' not in data or not isinstance(data['extra_data'], dict):
        data['extra_data'] = {}

    # Inject both hf_token and model_family into extra_data
    data['extra_data']['hf_token'] = hf_token
    data['extra_data']['model_family'] = model_family

    task_id = orchestrator.send_user_input(data)
    if task_id is None:
        return jsonify({'error': 'Invalid input data'}), 400
    return jsonify({'message': 'User input data sent successfully.',
                    'task_id': utils.task_json(task_id)}), 200


# Handle configuration request from the front-end
@server.route('/config_request', methods=['POST'])
def config_request():
    data = request.json
    res = orchestrator.send_request(data)
    if res is None:
        return jsonify({'error': 'Invalid input data'}), 400
    return jsonify({'message': 'Configuration request sent successfully.',
                    'response': utils.response_json(res)}), 200


@server.route('/hf_models_info', methods=['POST'])
def hf_models_info():
    data = request.json or {}
    model_ids = data.get("model_ids") or []
    if not isinstance(model_ids, list) or len(model_ids) == 0:
        return jsonify({"error": "model_ids must be a non-empty list"}), 400

    token = os.getenv("HF_TOKEN")  # reuse same env you already use

    models_out = []
    errors = []

    for mid in model_ids:
        try:
            status, info_json = _fetch_hf_model_info(mid, token)
            models_out.append({
                "model_id": mid,
                "status": status,
                "info": info_json if info_json is not None else {},
            })
        except requests.exceptions.ReadTimeout:
            print(f"[HF_MODELS_INFO] timeout {mid}", flush=True)
            errors.append({"model_id": mid, "error": "read_timeout"})
            models_out.append({
                "model_id": mid,
                "status": 0,
                "info": {},
                "error": "read_timeout",
            })
        except Exception as e:
            errors.append({"model_id": mid, "error": str(e)})
            models_out.append({
                "model_id": mid,
                "status": 0,
                "info": {},
                "error": str(e),
            })

    return jsonify({"models": models_out, "errors": errors}), 200


@server.route('/hf_models_compare', methods=['POST'])
def hf_models_compare():
    """
    Expects:
    {
      "models": [ ... ]   # output items from /hf_models_info
      OR
      "model_ids": [ ... ] # optional: you can implement fetch here too
    }
    """
    data = request.json or {}

    models = data.get("models")
    if not isinstance(models, list) or not models:
        return jsonify({"error": "models must be a non-empty list (use output from /hf_models_info)"}), 400

    # Limit to 5
    models = models[:5]

    token = os.getenv("HF_TOKEN")
    facts = [_model_facts(m, token) for m in models]

    prompt = _build_compare_prompt(facts)
    print(f"[COMPARE] prompt_len={len(prompt)}", flush=True)

    try:
        llm_text = _ollama_chat(prompt, model="llama3")
    except Exception as e:
        print(f"[COMPARE] Ollama ERROR: {e}", flush=True)
        return jsonify({"error": f"LLM call failed: {e}"}), 500

    # Parse returned JSON safely
    try:
        llm_text = _sanitize_json_text(llm_text)
        result = json.loads(llm_text)
    except Exception:
        # If model returns extra text, try to extract JSON
        m = re.search(r"\{.*\}", llm_text, flags=re.DOTALL)
        if not m:
            print(f"[COMPARE] LLM did not return JSON. raw_prefix={llm_text[:200]}", flush=True)

            return jsonify({"error": "LLM did not return JSON", "raw": llm_text[:2000]}), 500
        result = json.loads(m.group(0))

    # ALWAYS provide a stable, complete table from facts
    result["comparison_table"] = [_table_row_from_fact(f) for f in facts]
    # Ensure model_cards has EXACTLY 1 entry per selected model_id
    cards = result.get("model_cards") or []
    by_id = {c.get("model_id"): c for c in cards if isinstance(c, dict) and c.get("model_id")}

    fixed_cards = []
    for f in facts:
        mid = f.get("model_id") or ""
        c = by_id.get(mid)

        if not c:
            c = {
                "model_id": mid,
                "purpose": "Not enough Hugging Face metadata to summarize this model.",
                "best_for": ["—"],
                "watch_out_for": ["—"],
            }
        else:
            if not (c.get("purpose") or "").strip():
                c["purpose"] = "Not enough Hugging Face metadata to summarize this model."
            if not c.get("best_for"):
                c["best_for"] = ["—"]
            if not c.get("watch_out_for"):
                c["watch_out_for"] = ["—"]

        fixed_cards.append(c)

    result["model_cards"] = fixed_cards

    return jsonify(result), 200


# Retrieve Node status methods
@server.route('/status', methods=['GET'])
def status():
    return jsonify({'status': orchestrator.get_all_status()}), 200


@server.route('/status', methods=['POST'])
def status_args():
    data = request.json
    node_id = data.get('node_id')
    return jsonify({'status': orchestrator.get_status(node_id)}), 200


# Retrieve Node results methods
@server.route('/results', methods=['GET'])
def results():
    last_task_id = orchestrator.get_last_task_id()
    if last_task_id is None:
        return 'Nodes have not reported any output yet.\n', 200
    app_req = orchestrator.get_results(utils.node_id.APP_REQUIREMENTS.value, last_task_id)
    metadata = orchestrator.get_results(utils.node_id.ML_MODEL_METADATA.value, last_task_id)
    constraints = orchestrator.get_results(utils.node_id.HW_CONSTRAINTS.value, last_task_id)
    model = orchestrator.get_results(utils.node_id.ML_MODEL_PROVIDER.value, last_task_id)
    hardware = orchestrator.get_results(utils.node_id.HW_PROVIDER.value, last_task_id)
    carbontracker = orchestrator.get_results(utils.node_id.CARBONTRACKER.value, last_task_id)
    task_json = {'problem_id': last_task_id.problem_id(), 'iteration_id': last_task_id.iteration_id()}
    json = {utils.string_node(utils.node_id.APP_REQUIREMENTS.value): app_req,
            utils.string_node(utils.node_id.ML_MODEL_METADATA.value): metadata,
            utils.string_node(utils.node_id.HW_CONSTRAINTS.value): constraints,
            utils.string_node(utils.node_id.ML_MODEL_PROVIDER.value): model,
            utils.string_node(utils.node_id.HW_PROVIDER.value): hardware,
            utils.string_node(utils.node_id.CARBONTRACKER.value): carbontracker,
            'task_id': task_json}
    return jsonify(json), 200


@server.route('/results', methods=['POST'])
def results_args():
    data = request.json
    node_id = data.get('node_id')
    json_task = data.get('task_id')
    if json_task is not None:
        task_id = sustainml_swig.set_task_id(json_task.get('problem_id', 0), json_task.get('iteration_id', 0))
    else:
        task_id = None

    # Case of returning all nodes results. 9 = ALL
    if node_id == 9:
        app_req = orchestrator.get_results(utils.node_id.APP_REQUIREMENTS.value, task_id)
        metadata = orchestrator.get_results(utils.node_id.ML_MODEL_METADATA.value, task_id)
        constraints = orchestrator.get_results(utils.node_id.HW_CONSTRAINTS.value, task_id)
        model = orchestrator.get_results(utils.node_id.ML_MODEL_PROVIDER.value, task_id)
        hardware = orchestrator.get_results(utils.node_id.HW_PROVIDER.value, task_id)
        carbontracker = orchestrator.get_results(utils.node_id.CARBONTRACKER.value, task_id)
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        json = {utils.string_node(utils.node_id.APP_REQUIREMENTS.value): app_req,
                utils.string_node(utils.node_id.ML_MODEL_METADATA.value): metadata,
                utils.string_node(utils.node_id.HW_CONSTRAINTS.value): constraints,
                utils.string_node(utils.node_id.ML_MODEL_PROVIDER.value): model,
                utils.string_node(utils.node_id.HW_PROVIDER.value): hardware,
                utils.string_node(utils.node_id.CARBONTRACKER.value): carbontracker,
                'task_id': task_json}
        return jsonify(json), 200

    return jsonify({utils.string_node(node_id): orchestrator.get_results(node_id, task_id)}), 200


# Flask server shutdown route
@server.route('/shutdown', methods=['GET'])
def shutdown():
    shutdown_func = request.environ.get('werkzeug.server.shutdown')
    if shutdown_func is None:
        return jsonify({'message': 'Use "/terminate" route to stop Back-end node.<br>'}), 200
    shutdown_func()
    return jsonify({'message': 'Terminating...<br>'}), 200


# Flask server terminate route
@server.route('/terminate', methods=['GET'])
def terminate():
    signal.raise_signal(signal.SIGINT)
    return jsonify({'message': 'Terminating...<br>'}), 200


class ServerThread(threading.Thread):
    def __init__(self):
        # Create orchestrator node
        self.orchestrator_thread = threading.Thread(target=orchestrator.run)
        # Create Flask server
        threading.Thread.__init__(self)
        self.srv = make_server(server_ip_address, server_port, server)
        self.ctx = server.app_context()
        self.ctx.push()

    def run(self):
        # Start the orchestrator thread
        self.orchestrator_thread.start()
        # Start the Flask server
        self.srv.serve_forever()

    def shutdown(self):
        # Terminate the orchestrator and its thread
        orchestrator.terminate()
        # Terminate the Flask server
        self.srv.shutdown()
        # Wait for the orchestrator thread to finish
        self.orchestrator_thread.join()


# Process signal handler
def signal_handler(sig, frame):
    print("\nBack-end node terminating...")
    flask_server_thread.shutdown()
    global running
    running = False


# Main program execution
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    hf_token = os.getenv("HF_TOKEN")
    if hf_token is None:
        print("Error: The HF_TOKEN environment variable is missing. Please set it before starting the node.")
        # sys.exit(0)
    print("Back-end Node running, use Ctrl+C to terminate. Server listening at http://" + server_ip_address + ":" + str(server_port) + "/")
    flask_server_thread = ServerThread()
    flask_server_thread.start()  # Start the Flask server with the orchestrator
    while running:
        time.sleep(1)
    flask_server_thread.join()
    sys.exit(0)
