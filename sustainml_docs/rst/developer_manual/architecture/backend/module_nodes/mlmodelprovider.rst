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

The :ref:`mlmodelprovider_node` is responsible for choosing the machine learning model that solves the problem defined by the user, for the goal selected by the :ref:`mlmodelmetadata_node` and the hardware chosen by the user.
It also answers the requests of the :ref:`frontend` for the list of models of a goal and for the search of models in the Hugging Face Hub.

The node implementation is the ``ml_model_provider_node.py`` file of the WP1 module (``sustainml_modules/sustainml_modules/sustainml-wp1``).

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`mlmodelprovider_node`:

+--------------------------------+---------------------------+---------------------------+---------------------------+
| Input                          | From Node                 | Output                    | To Node                   |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodelmetadata_type`     |:ref:`mlmodelmetadata_node`|:ref:`mlmodel_type`        |:ref:`hwprovider_node`     |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_constraints_type`|:ref:`hwconstraints_node`  |:ref:`mlmodel_type`        |:ref:`carbontracker_node`  |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`apprequirements_type`     |:ref:`appreqs_node`        |:ref:`mlmodel_type`        |:ref:`orchestrator`        |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodel_type` (baseline)  |:ref:`orchestrator`        |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_resource_type`   |:ref:`orchestrator`        |                           |                           |
|(baseline)                      |                           |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`carbonfootprint_type`     |:ref:`orchestrator`        |                           |                           |
|(baseline)                      |                           |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+

The node only starts a task when it has received the six inputs of that task.
The three baselines are published by the :ref:`orchestrator` when a task or a new iteration starts, and hold the outputs of this node, the :ref:`hwprovider_node` and the :ref:`carbontracker_node` in the previous iteration of the problem, if any.
The current implementation does not use the :ref:`apprequirements_type` nor the baselines: it relies on the ``extra_data`` of the :ref:`mlmodelmetadata_type`, on the ``hardware_required`` and ``max_memory_footprint`` of the :ref:`hardware_constraints_type`, and on the ``model_restrains`` that the :ref:`orchestrator` carries between iterations.

From the :ref:`mlmodelmetadata_type`, the node reads the goal (first element of ``ml_model_metadata``) and these keys of the ``extra_data`` JSON, written by the :ref:`mlmodelmetadata_node`:

* ``problem_short_description``: used to search the model.
* ``model_selected``: the model chosen by hand in the :ref:`frontend`, if any.
* ``model_restrains``: the models that must not be suggested again.
* ``type``: the model family (for instance ``Transformers`` or ``CNNs``).
* ``hardware_required``: the hardware, used when the :ref:`hardware_constraints_type` has none.
* ``yolo_preferences``: the requirements read from the problem description for the YOLO models of the Axelera board.

In the :ref:`mlmodel_type` output, the node fills:

* ``model``: the name of the chosen model (a Hugging Face model id, a YOLO model of the Axelera board or a U-Net model of the FPGA).
* ``model_path``: the path to the ONNX file of the model, only for a U-Net model chosen by hand; ``""`` for any other model.
* ``extra_data``: ``{"unsupported_models": [...]}``, the list of the ``unsupported_models.txt`` file of the WP1 module (``llama``, ``mistral``, ``qwen``, ``bloom``, ``t5``, ``phi3`` and ``idefics2``).
  The :ref:`hwprovider_node` and the :ref:`carbontracker_node` refuse to load a model whose class name contains any of these words.

If no model can be chosen, the output is ``model`` ``NO_MODEL``, ``model_path`` ``N/A`` and ``extra_data`` ``{"error_code": "NO_MODEL", "error": "No suitable model found for the given problem."}``.

Model selection
---------------

The way the model is chosen depends on the model family and the hardware chosen by the user:

**CNNs on the Axelera board**

When the ``type`` is ``CNNs`` and the hardware (the first element of ``hardware_required`` in the :ref:`hardware_constraints_type`) is ``Axelera Edge 130p``, the model is one of the YOLO models of the board,
listed in the YOLO catalog of the WP2 module (``sustainml-wp2/hw_provider_axelera/yolo_models.json``), with their accuracy and the latency, throughput, memory and power measured on the board.

* If a model was chosen by hand, it is used, provided that it is in the catalog.
* Otherwise, the YOLO models of the goal are ranked, and the first one is chosen.
  The ranking leaves out the models in ``model_restrains`` and those whose memory exceeds the ``max_memory_footprint`` of the :ref:`hardware_constraints_type`,
  keeps only the models that reach the ``min_fps`` and ``max_latency_ms`` of the ``yolo_preferences`` (if none does, the fastest one is chosen),
  and orders them by the ``priority`` of the ``yolo_preferences``: the most accurate after the int8 quantization, the fastest, the one with the lowest energy per inference, or the best balance between accuracy and energy.

**Model chosen by hand**

For any other family or hardware, a model chosen by hand (``model_selected``) is used as it is.
If it is the path to an existing ``.onnx`` file, or the name of one of the U-Net models of the FPGA, ``model_path`` is set to its ONNX file.
The U-Net models are the ONNX files in the ``SUSTAINML_UNET_DIR`` directory if that environment variable is set, or else in ``sustainml-wp2/hw_provider_fpga/vendor/sustain_ml_predictor/unet_models``.

**Automatic selection among the models of the graph**

Otherwise, the model is chosen among the models that the Neo4j graph links to the goal:

#. The models of the goal are retrieved from the graph, ordered by downloads.
   For the ``summarization``, ``translation`` and ``text-generation`` goals, they are restricted to a whitelist of models defined in ``rdftool/rdfCode.py``.
#. The models in ``model_restrains`` are left out, so that a model is not suggested twice for the same problem.
#. The RAG back-end of the WP1 module (``rag/rag_backend.py``) picks one of the remaining models for the query ``Task <goal> with problem description: <problem_short_description>?``:
   the query is embedded with the ``BAAI/bge-large-en`` sentence-transformers model and compared with the embeddings of the candidate models, stored in the ``models_index.ann`` Annoy index.
   The nearest of them is proposed, so the same query always gives the same model.
#. The model is accepted if the graph links it to the goal.
   Otherwise it is added to the excluded models and the RAG is asked again, also when no model is returned.
   The RAG is asked up to 10 times in total.

In this case ``model_path`` is ``""``: the :ref:`hwprovider_node` and the :ref:`carbontracker_node` download the model from the Hugging Face Hub by its name.

Reiterations
^^^^^^^^^^^^

Every iteration of a problem runs the selection again with the ``model_restrains`` of the :ref:`mlmodelmetadata_type`, so that a different model is suggested each time:

* The :ref:`carbontracker_node` adds the evaluated model to the ``model_restrains`` of its output, and the :ref:`orchestrator` copies them to the user input of the next iteration when it reiterates by itself:
  when the user asks for more than one output model, and when the automatic carbon footprint optimization discards a model that misses the desired carbon footprint or the maximum memory footprint.
* When the user reiterates a problem with the manual or automatic carbon footprint optimization enabled (``optimize_carbon_footprint_manual`` or ``optimize_carbon_footprint_auto``)
  and no model is chosen by hand, the :ref:`orchestrator` adds to ``model_restrains`` every model already suggested in the problem.
* When a model is chosen by hand, the :ref:`orchestrator` sets the number of output models to 1, as more outputs would only repeat it.

Configuration Requests
----------------------

The :ref:`orchestrator` routes to this node every :ref:`configuration request <node_config_service>` whose ``configuration`` string contains ``model_from_goal`` or ``hf_search``.
The node answers the following requests:

+------------------------------------------+-----------------------------------------------------+---------------------------------------------+
| Request ``configuration``                | Description                                         | Reply ``configuration``                     |
+==========================================+=====================================================+=============================================+
| ``model_from_goal, <goal>[, <hardware>`` | List of the models that the user can choose for a   | ``{"models": "model_1, model_2, ..."}``     |
| ``[, <family>]]``                        | goal, a hardware and a model family.                |                                             |
+------------------------------------------+-----------------------------------------------------+---------------------------------------------+
| ``hf_search, <description>, <limit>``    | Search of the Hugging Face Hub for the models that  | ``{"models": [...], "task": "...",``        |
|                                          | best match a description.                           | ``"pipeline_tag": "...", "candidates": N}`` |
+------------------------------------------+-----------------------------------------------------+---------------------------------------------+

Any other request is answered with ``success`` set to ``false`` and ``{"models": ""}``.

model_from_goal
^^^^^^^^^^^^^^^

The ``models`` of the reply are a single string with the model names separated by ``", "``.
The list depends on the arguments of the request:

* If the family is ``CNNs`` and the hardware is ``Axelera Edge 130p``: the YOLO models of the goal, in the order of the YOLO catalog.
* Else, if the goal is ``U_NET_MODELS``, or the family is ``CNNs`` and the hardware contains ``FPGA``: the names, sorted and without the ``.onnx`` extension, of the U-Net models of the FPGA.
  ``U_NET_MODELS`` is the value used by the :ref:`frontend` to ask for them.
* Otherwise: the models that the Neo4j graph links to the goal (filtered by the whitelist of the goal, if any), sorted by name.

``success`` is ``false`` when the list of YOLO or graph models is empty.

hf_search
^^^^^^^^^

This request browses the Hugging Face Hub to show models to the user; the models found are not evaluated by the framework.
The ``<description>`` may contain commas: the ``<limit>``, the maximum number of models returned (20 if it is missing or not a number), is taken after the last comma.

The search is done by the ``hf_search_models`` tool of the Hugging Face MCP server of the WP1 module (``hf_mcp_server.py``), built with `FastMCP <https://gofastmcp.com/>`_.
The node starts the server, as a subprocess that talks to it through ``stdio``, the first time it receives a request, and keeps it running until it terminates.
The tool:

#. Infers the goal of the description among ``summarization``, ``translation`` and ``text-generation`` with the ``llama3`` LLM served by Ollama (up to 3 attempts).
   If it cannot, it returns ``{"models": [], "error": "could_not_infer_goal"}``.
#. Gathers the candidates from the Hugging Face Hub: the models with that ``pipeline_tag`` with the most downloads, with the most likes and with the most recent changes (a third of ``max(3 * limit, 200)`` from each list), without repetitions.
#. Ranks them by the similarity between the description and their model card, with ``BAAI/bge-large-en`` embeddings (the ``SUSTAINML_EMB_MODEL`` environment variable sets another sentence-transformers model).
   The embeddings of the models are cached in ``~/.cache/sustainml/hf_model_emb_cache.jsonl``.
   If the embedding model cannot be loaded, the candidates are ranked by downloads, likes and last change instead.
#. Returns the first ``limit`` models, each one with its ``model_id``, ``author``, ``downloads``, ``likes``, ``pipeline_tag``, ``library_name``, ``tags``, ``cardData``, ``card_text``, ``sha``, ``lastModified``, ``url``, ``score`` (when the embedding model is available) and a ``tooltip`` text that summarizes its model card.

When the ``<description>`` is ``__MODEL_CONFIG__:<model_id>``, the tool returns instead ``{"model_id": "...", "tooltip": "..."}``, a summary of the ``config.json`` file of that model (architecture, layers, hidden size, vocabulary, and so on).
The :ref:`frontend` uses it for the tooltip shown when hovering a model.

The node tries the call up to 3 times, with a timeout of 2 minutes each.
If all of them fail, the reply is ``{"models": [], "error": "<error message>"}`` with ``success`` set to ``false``.
An error returned by the tool itself, such as ``could_not_infer_goal``, is passed on in the reply with ``success`` set to ``true``.

External dependencies
---------------------

* **Neo4j**: the knowledge graph of models and goals, reached at ``bolt://localhost:7687`` (user ``neo4j``, password ``12345678``).
  The database is loaded from the dump files in ``sustainml-wp1/rag/neo4j_backup`` with ``neo4j-admin database load``, as explained in the :ref:`installation_framework`.
  At start-up, the node waits up to 5 seconds for the graph to answer, and exits if it does not.
* **RAG back-end**: loaded when the node starts, it needs the ``models_index.ann`` Annoy index with the embeddings of the models of the graph, and their names in ``rag/model_metadata.json``.
  The index is looked for next to ``rag_backend.py`` and in ``~/.cache/sustainml``; if it is in neither, it is downloaded from Google Drive into ``~/.cache/sustainml``.
  The ``BAAI/bge-large-en`` model is downloaded from the Hugging Face Hub the first time, and runs on the GPU if CUDA is available.
* **Ollama**: the ``llama3`` model, served at ``http://localhost:11434``, used by the Hugging Face MCP server to infer the goal of a search.
* **Hugging Face Hub**: queried by the MCP server for the ``hf_search`` request.
* **WP2 module**: the ``hw_provider_axelera`` package of ``sustainml-wp2``, for the YOLO catalog and ranking, and the ``hw_provider_fpga`` package, which holds the U-Net models.

Node Template
-------------

Following is the Python API provided for the :ref:`mlmodelprovider_node`.
User is meant to implement the functionality of the node within the ``task_callback()``.
And inside ``configuration_callback()`` implement the response to the configuration request from the orchestrator.

The template below is a shortened version of ``sustainml-wp1/ml_model_provider_node.py`` (`full file on GitHub <https://github.com/eProsima/sustainml-wp1/blob/main/ml_model_provider_node.py>`__).
It keeps the structure of the node and the way it reads and writes its data, and picks the most downloaded model of the goal that has not been suggested yet. It leaves out, as described above:

* CNNs on the Axelera board: the YOLO model picked for the goal and the description, or the selected one checked against the catalog;
* the ONNX file of a selected U-Net model (``model_path``), looked up in the ``unet_models`` folder or ``SUSTAINML_UNET_DIR``;
* the automatic selection with the RAG back-end (embeddings, nearest models, up to 10 attempts, and the check that the graph links the model to the goal);
* the unsupported model architectures, read from ``unsupported_models.txt``;
* the replies to ``model_from_goal`` for the YOLO models and for the U-Net models of the FPGA (``U_NET_MODELS``), and the ``hf_search`` request, served by the MCP server.

.. code-block:: python

    # Copyright 2026 SustainML Consortium
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

    from sustainml_py.nodes.MLModelNode import MLModelNode

    # Manage signaling
    import signal
    import threading
    import time
    import json

    from rdftool.rdfCode import load_graph, get_models_for_problem

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        MLModelNode.terminate()
        global running
        running = False

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

        try:
            # The extra_data written by the ML Model Metadata node
            extra_data_str = ''.join(chr(b) for b in ml_model_metadata.extra_data())
            extra_data_dict = json.loads(extra_data_str) if extra_data_str else {}

            chosen_model = extra_data_dict.get("model_selected")
            if not chosen_model:
                # Goal selected by the ML Model Metadata node
                goal = ml_model_metadata.ml_model_metadata()[0]
                restrained_models = extra_data_dict.get("model_restrains", [])
                candidates = [name for (name, _) in get_models_for_problem(goal)
                              if name not in restrained_models]
                if not candidates:
                    raise Exception("No candidates in graph for the selected goal")
                chosen_model = candidates[0]

            ml_model.model(chosen_model)
            ml_model.model_path("")
            ml_model.extra_data(json.dumps({"unsupported_models": []}).encode("utf-8"))

        except Exception as e:
            print(f"[WARN] No suitable model found for task {ml_model_metadata.task_id()}: {e}")
            ml_model.model("NO_MODEL")
            ml_model.model_path("N/A")
            error_info = {"error_code": "NO_MODEL", "error": "No suitable model found for the given problem."}
            ml_model.extra_data(json.dumps(error_info).encode("utf-8"))

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        raw = (req.configuration() or "").strip()
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())

        # "model_from_goal, <goal>[, <hardware>[, <family>]]"
        if raw.lower().startswith("model_from_goal"):
            parts = [p.strip() for p in raw.split(",")[1:]]
            goal = parts[0] if parts else ""
            names = sorted(name for (name, _) in get_models_for_problem(goal))
            res.success(bool(names))
            res.err_code(0 if names else 1) # 0: No error || 1: Error
            res.configuration(json.dumps({"models": ", ".join(names)}))
            return

        print(f"[ML_MODEL_PROVIDER] Unsupported configuration request: {raw}")
        res.success(False)
        res.err_code(1) # 0: No error || 1: Error
        res.configuration(json.dumps({"models": ""}))

    # Main workflow routine
    def run():
        # Wait for the Neo4j graph
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
