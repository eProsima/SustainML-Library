.. _mlmodelprovider_node:

ML-Model-Provider Node
======================

.. raw:: html

   <style>
        .module_node:not(#ml-model-provider) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#ml-model-provider) {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../../../figures/sustainml_framework_arch.svg

The :ref:`mlmodelprovider_node` is responsible for estimating the carbon footprint of the machine learning model on the selected hardware.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`mlmodelprovider_node`:

+--------------------------------+---------------------------+---------------------------+---------------------------+
| Input                          | From Node                 | Output                    | To Node                   |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodelmetadata_type`     |:ref:`mlmodelmetadata_node`|:ref:`mlmodel_type`        |:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_constraints_type`|:ref:`hwconstraints_node`  |:ref:`mlmodel_type`        |:ref:`carbontracker_node`  |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`apprequirements_type`     |:ref:`appreqs_node`        |:ref:`mlmodel_type`        |:ref:`orchestrator`        |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_resource_type`   |:ref:`hwprovider_node`     |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`carbonfootprint_type`     |:ref:`carbontracker_node`  |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`mlmodelprovider_node`.
User is meant to implement the funcionality of the node within the ``test:callback()``.
And inside ``configuration_callback()`` implement the response to the configuration request from the orchestrator.

.. code-block:: python

    # Copyright 2023 SustainML Consortium
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
    """SustainML ML Model Provider Node Implementation."""

    import asyncio
    import json
    import os
    import signal
    import sys
    import threading
    import time

    HERE = os.path.dirname(__file__)
    WP2_ROOT = os.path.abspath(os.path.join(HERE, "..", "sustainml-wp2"))

    if WP2_ROOT not in sys.path:
        sys.path.insert(0, WP2_ROOT)

    import hw_provider_fpga

    from rdftool.ModelONNXCodebase import model
    from neo4j import GraphDatabase
    from rdftool.rdfCode import load_graph, get_models_for_problem
    from rag.rag_backend import answer_question
    from os.path import isdir, dirname, abspath, join
    from os import listdir
    from sustainml_py.nodes.MLModelNode import MLModelNode
    from fastmcp import Client


    MCP_SERVER_SCRIPT = os.path.abspath(os.path.expanduser(
        "~/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/hf_mcp_server.py"
    ))

    _mcp_loop = None
    _mcp_thread = None
    _mcp_client = None
    _mcp_client_ctx = None


    def _start_asyncio_loop():
        global _mcp_loop
        _mcp_loop = asyncio.new_event_loop()
        asyncio.set_event_loop(_mcp_loop)
        _mcp_loop.run_forever()


    _mcp_thread = threading.Thread(target=_start_asyncio_loop, daemon=True)
    _mcp_thread.start()


    # Create (once) and return a persistent FastMCP client connected to the HF MCP server
    async def _ensure_mcp_client():
        global _mcp_client, _mcp_client_ctx

        if _mcp_client is not None:
            return _mcp_client

        config = {
            "mcpServers": {
                "hf": {
                    "command": sys.executable,
                    "args": ["-u", MCP_SERVER_SCRIPT],
                }
            }
        }

        # Keep the context open forever
        _mcp_client_ctx = Client(config)
        _mcp_client = await _mcp_client_ctx.__aenter__()
        return _mcp_client


    # Calls a Hugging Face MCP tool asynchronously from the ML Model Provider node
    def _mcp_call(tool_name: str, args: dict) -> dict:
        async def _run():
            c = await _ensure_mcp_client()
            res = await c.call_tool(f"hf_{tool_name}", args)
            return json.loads(res.content[0].text)

        # Submit coroutine to background loop
        fut = asyncio.run_coroutine_threadsafe(_run(), _mcp_loop)
        return fut.result(timeout=120) # 2 minutes timeout for MCP call


    # Neo4j config/driver for local checks (used by _model_has_goal)
    NEO4J_URI = "bolt://localhost:7687"
    NEO4J_USER = "neo4j"
    NEO4J_PASSWORD = "12345678"
    neo4j_driver = GraphDatabase.driver(NEO4J_URI, auth=(NEO4J_USER, NEO4J_PASSWORD))


    # Return the directory containing vendored U-Net ONNX models
    def _unet_models_dir():
        env = os.environ.get("SUSTAINML_UNET_DIR")
        if env:
            return os.path.abspath(env)

        return os.path.abspath(
            os.path.join(os.path.dirname(hw_provider_fpga.__file__),
                        "vendor", "sustain_ml_predictor", "unet_models")
        )


    # Lists available U-Net .onnx models from the vendor directory
    def list_unet_onnx_models(basename_only=True):
        d = _unet_models_dir()
        if not os.path.isdir(d):
            print(f"[WARN] U-Net directory not found: {d}")
            return []
        names = [f for f in os.listdir(d) if f.endswith(".onnx")]
        return sorted([os.path.splitext(f)[0] for f in names]) if basename_only else \
            sorted([os.path.join(d, f) for f in names])


    # Checks in Neo4j whether a model node is linked to the given goal/problem
    def _model_has_goal(neo4j_driver, model_name: str, goal: str) -> bool:
        cypher = """
        MATCH (m:Model {name: $model})-[:HAS_PROBLEM]->(p:Problem)
        WHERE toLower(p.name) = toLower($goal)
        RETURN COUNT(*) AS cnt
        """
        with neo4j_driver.session() as s:
            r = s.run(cypher, model=model_name, goal=goal).single()
            return bool(r and r["cnt"] > 0)

    # Whether to go on spinning or interrupt
    running = False


    # Load the list of unsupported models
    def load_unsupported_models(file_path):
        try:
            with open(file_path, 'r') as f:
                return [line.strip().lower() for line in f if line.strip()]
        except Exception as e:
            print(f"[WARN] Could not load unsupported list: {e}")
            return []


    unsupported_models = load_unsupported_models(os.path.dirname(__file__) + "/unsupported_models.txt")


    # Close the FastMCP client context and reset MCP globals
    def _shutdown_mcp():
        global _mcp_client_ctx, _mcp_client
        async def _close():
            global _mcp_client_ctx, _mcp_client
            if _mcp_client_ctx is not None:
                try:
                    await _mcp_client_ctx.__aexit__(None, None, None)
                except Exception:
                    pass
            _mcp_client_ctx = None
            _mcp_client = None

        if _mcp_loop:
            try:
                asyncio.run_coroutine_threadsafe(_close(), _mcp_loop).result(timeout=5)
            except Exception:
                pass

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        MLModelNode.terminate()
        global running
        running = False
        _shutdown_mcp()


    # User Callback implementation
    # Inputs: ml_model_metadata, app_requirements, hw_constraints, ml_model_baseline, hw_baseline, carbonfootprint_baseline
    # Outputs: node_status, ml_model
    def task_callback(ml_model_metadata,
                    app_requirements,
                    hw_constraints,
                    ml_model_baseline,
                    hw_baseline,
                    carbonfootprint_baseline,
                    node_status,
                    ml_model):

        # Callback implementation here

        print(f"Received Task: {ml_model_metadata.task_id().problem_id()},{ml_model_metadata.task_id().iteration_id()}")

        try:
            chosen_model = None
            # Model restriction after various outputs
            restrained_models = []
            type = None
            extra_data_bytes = ml_model_metadata.extra_data()
            if extra_data_bytes:
                extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
                try:
                    extra_data_dict = json.loads(extra_data_str)
                except json.JSONDecodeError:
                    print("[WARN] In model_provider node extra_data JSON is not valid.")
                    extra_data_dict = {}

                if "type" in extra_data_dict:
                    type = extra_data_dict["type"]

                if "model_restrains" in extra_data_dict:
                    restrained_models = extra_data_dict["model_restrains"]

                if "model_selected" in extra_data_dict:
                    chosen_model = extra_data_dict["model_selected"]
                    print("Model already selected: ", chosen_model)

                # If a model was manually selected, skip automatic selection
                if chosen_model:

                    # If user passed a full path to an .onnx, use it directly
                    onnx_path = None
                    if isinstance(chosen_model, str) and chosen_model.endswith(".onnx") and os.path.isfile(chosen_model):
                        onnx_path = chosen_model

                    # Else, try U-Net vendor dir: <basename>[.onnx]
                    if onnx_path is None:
                        unet_dir = _unet_models_dir()
                        cand1 = os.path.join(unet_dir, chosen_model)          # maybe includes .onnx already
                        cand2 = os.path.join(unet_dir, chosen_model + ".onnx")
                        if os.path.isfile(cand1):
                            onnx_path = cand1
                        elif os.path.isfile(cand2):
                            onnx_path = cand2

                    # Else, fall back to graph name -> onnx path resolution
                    if onnx_path is None:
                        onnx_path = model(chosen_model)

                    ml_model.model(chosen_model)
                    ml_model.model_path(onnx_path)

                    # Add unsupported_models information to extra_data in JSON format
                    extra_data = {"unsupported_models": unsupported_models}
                    encoded_data = json.dumps(extra_data).encode("utf-8")
                    ml_model.extra_data(encoded_data)
                    return

                problem_short_description = extra_data_dict["problem_short_description"]

            goal = ml_model_metadata.ml_model_metadata()[0]  # Goal selected by metadata node
            print(f"Problem short description: {problem_short_description}")
            print(f"Selected goal (metadata): {goal}")

            # Build strictly goal-scoped allowed list (names only)
            goal_models = get_models_for_problem(goal)   # [(model_name, downloads), ...]
            allowed_names = [name for (name, _) in goal_models]
            if not allowed_names:
                raise Exception("No candidates in graph for the selected goal")

            # Track models to avoid repeats across outputs
            restrained_models = []
            if extra_data_bytes:
                try:
                    if "model_restrains" in extra_data_dict:
                        restrained_models = list(set(extra_data_dict["model_restrains"]))
                except Exception:
                    pass

            # Try up to 10 candidates, skipping misfits transparently
            chosen_model = None
            for _ in range(10):
                remaining = [n for n in allowed_names if n not in restrained_models]
                if not remaining:
                    break

                candidate = answer_question(
                    f"Task {goal} with problem description: {problem_short_description}?",
                    allowed_models=remaining
                )

                if not candidate or candidate.strip().lower() == "none":
                    # Mark and try again
                    if candidate:
                        restrained_models.append(candidate)
                    continue

                # Final safety: ensure candidate really belongs to goal
                if not _model_has_goal(neo4j_driver, candidate, goal):
                    restrained_models.append(candidate)
                    continue

                chosen_model = candidate
                break

            if not chosen_model:
                raise Exception("No suitable model after screening candidates")
            print(f"ML Model chosen: {chosen_model}")

            # Generate model code and keywords
            onnx_path = model(chosen_model)     # TODO - Further development needed
            ml_model.model(chosen_model)
            ml_model.model_path(onnx_path)
            # Add unsupported_models information to extra_data in json format
            extra_data = {"unsupported_models": unsupported_models}
            encoded_data = json.dumps(extra_data).encode("utf-8")
            ml_model.extra_data(encoded_data)

        except Exception as e:
            print(f"[WARN] No suitable model found for task {ml_model_metadata.task_id()}: {e}")
            ml_model.model("NO_MODEL")
            ml_model.model_path("N/A")
            error_message = "No suitable model found for the given problem."
            error_info = {"error_code": "NO_MODEL", "error": error_message}
            encoded_error = json.dumps(error_info).encode("utf-8")
            ml_model.extra_data(encoded_error)


    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        raw = (req.configuration() or "").strip()
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())

        # HF search (metadata-only browsing; no evaluation)
        if raw.lower().startswith("hf_search"):
            # Expected: "hf_search, <description>, <limit>"
            rest = raw[len("hf_search"):].lstrip()
            if rest.startswith(","):
                rest = rest[1:].lstrip()

            # Split from the RIGHT so commas inside description are allowed
            description = rest
            limit = 20

            if "," in rest:
                desc_part, limit_part = rest.rsplit(",", 1)
                description = desc_part.strip()
                try:
                    limit = int(limit_part.strip())
                except Exception:
                    limit = 20
            else:
                description = rest.strip()

            try:
                resp = _mcp_call("search_models", {"description": description, "limit": limit})
                res.success(True)
                res.err_code(0)
                res.configuration(json.dumps(resp))
                return
            except Exception as e:
                res.success(False)
                res.err_code(1)
                res.configuration(json.dumps({"models": [], "error": str(e)}))
                return

        # Only handle the listing endpoint(s)
        if raw.lower().startswith("model_from_goal"):
            # Accept both forms:
            #   "model_from_goal, <goal>, <hw>, <family>"
            #   "model_from_goal, model_from_goal, <goal-or-sentinel>, <hw>, <family>" (buggy frontend)
            s = raw.split(",", 1)
            args_str = s[1] if len(s) > 1 else ""          # everything after first comma
            parts = [p.strip() for p in args_str.split(",") if p is not None]

            # Normalize arity (goal, hw, family)
            goal   = parts[0] if len(parts) >= 1 else ""
            hw     = parts[1] if len(parts) >= 2 else ""
            family = parts[2] if len(parts) >= 3 else ""

            fam_l = (family or "").lower()
            hw_l  = (hw or "").lower()
            is_cnn  = fam_l.lower() == "cnns"
            is_fpga = "fpga" in hw_l

            # U-Net fast path: allow sentinel goals like U_NET_MODELS or any goal when (FPGA+CNNs)
            if (goal.upper() == "U_NET_MODELS") or (is_cnn and is_fpga):
                try:
                    try:
                        vendored = abspath(join(dirname(hw_provider_fpga.__file__),
                                                "vendor", "sustain_ml_predictor", "unet_models"))
                    except Exception as e:
                        print(f"[ML_MODEL_PROVIDER] hw_provider_fpga not importable: {e}")
                        vendored = ""  # fall back to empty

                    names = []
                    if vendored and isdir(vendored):
                        names = [f[:-5] for f in listdir(vendored) if f.endswith(".onnx")]

                    csv = ", ".join(sorted(names))
                    res.success(True)
                    res.err_code(0)
                    res.configuration(json.dumps({"models": csv}))
                    return
                except Exception as e:
                    print(f"[ML_MODEL_PROVIDER] U-NET listing error: {e}")
                    res.success(False)
                    res.err_code(1)
                    res.configuration(json.dumps({"models": ""}))
                    return

            # Default path: fetch by real goal from Neo4j
            try:
                models = get_models_for_problem(goal)  # [(name, downloads)]
                names = [m for (m, _) in models]
                csv = ", ".join(sorted(names))
                res.success(bool(names))
                res.err_code(0 if names else 1)
                res.configuration(json.dumps({"models": csv}))
                return
            except Exception as e:
                print(f"[ML_MODEL_PROVIDER] Neo4j listing error: {e}")
                res.success(False)
                res.err_code(1)
                res.configuration(json.dumps({"models": ""}))
                return

        # Unsupported
        msg = f"Unsupported configuration request: {raw}"
        print(f"[ML_MODEL_PROVIDER] {msg}")
        res.success(False)
        res.err_code(1)
        res.configuration(json.dumps({"models": ""}))


    # Main workflow routine
    def run():
        start_time = time.time()
        loaded = False
        while time.time() - start_time < 5:
            if load_graph():
                loaded = True
                break
            time.sleep(0.1)
        if not loaded:
            print("[Error][ml_model_provider] Graph not available")
            exit(1)
        node = MLModelNode(callback=task_callback, service_callback=configuration_callback)
        global running
        running = True
        node.spin()


    # Call main in program execution
    if __name__ == '__main__':
        signal.signal(signal.SIGINT, signal_handler)

        """Python does not process signals async if
        the main thread is blocked (spin()) so, tun
        user work flow in another thread """
        runner = threading.Thread(target=run)
        runner.start()

        while running:
            time.sleep(1)

        runner.join()
