.. _mlmodelmetadata_node:

ML-Model-Metadata Node
======================

.. raw:: html

   <style>
        .module_node:not(#ml-model-metadata) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#ml-model-metadata) {
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

The :ref:`mlmodelmetadata_node` processes the problem description given by the user and selects the machine learning goal (the type of task, such as ``summarization`` or ``object-detection``) that the :ref:`mlmodelprovider_node` uses to choose the model.
It also answers the requests of the :ref:`frontend` for the list of goals and for the description of a dataset.

The node implementation is the ``ml_model_metadata_node.py`` file of the WP1 module (``sustainml_modules/sustainml_modules/sustainml-wp1``).

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`mlmodelmetadata_node`:

+----------------------+-------------------+--------------------------------+---------------------------+
| Input                | From Node         | Output                         | To Node                   |
+----------------------+-------------------+--------------------------------+---------------------------+
|:ref:`user_input_type`|:ref:`orchestrator`|:ref:`mlmodelmetadata_type`     |:ref:`mlmodelprovider_node`|
+----------------------+-------------------+--------------------------------+---------------------------+
|                      |                   |:ref:`mlmodelmetadata_type`     |:ref:`orchestrator`        |
+----------------------+-------------------+--------------------------------+---------------------------+

From the :ref:`user_input_type` the node reads ``problem_short_description``, ``problem_definition``, ``inputs``, ``outputs``, ``minimum_samples``, ``maximum_samples`` and the following keys of the ``extra_data`` JSON:

* ``goal``: the goal chosen in the :ref:`frontend`.
  The :ref:`orchestrator` also sets it to the goal of the previous iteration when it reiterates a problem by itself (more than one output model asked for, or the automatic carbon footprint optimization).
* ``model_selected``: the model chosen by hand in the :ref:`frontend`, if any.
* ``model_restrains``: the models that must not be suggested again.
* ``type`` and ``model_family``: the model family chosen in the :ref:`frontend` (for instance ``Transformers`` or ``CNNs``).
* ``hardware_required``: the hardware chosen in the :ref:`frontend`.
* ``dataset_metadata_description``, ``dataset_metadata_topic``, ``dataset_metadata_profile``, ``dataset_metadata_keywords`` and ``dataset_metadata_applications``: the description of the dataset loaded by the user, as returned by the ``dataset_path`` request explained below.

In the :ref:`mlmodelmetadata_type` output, the node fills:

* ``ml_model_metadata``: a list with the selected goal as its only element.
  It is left empty when a model was chosen by hand (no goal is needed then) and when no goal could be selected.
* ``extra_data``: a JSON object for the :ref:`mlmodelprovider_node` with the keys ``problem_short_description``, ``model_restrains``, ``model_selected``, ``type`` and ``hardware_required`` copied from the user input
  (``problem_short_description`` always, even when empty; ``model_restrains`` whenever present, even when empty; the others only if present and not empty),
  plus ``yolo_preferences`` for the YOLO models of the Axelera board (see below).
  If no goal could be selected, the ``extra_data`` is replaced by ``{"error": "Failed to extract metadata due to invalid or incomplete input parameters."}``.

The ``keywords`` field of the output is not filled.

Goal selection
--------------

For each task, the node selects the goal as follows:

#. If the user input carries a ``goal`` in its ``extra_data``, that goal is used as it is.
#. Otherwise, if a model was chosen by hand (``model_selected``), no goal is selected and the ``ml_model_metadata`` list is left empty.
#. Otherwise, the goal is inferred by the ``llama3`` LLM, served by `Ollama <https://ollama.com/>`_ at ``http://localhost:11434``.
   The prompt contains the problem short description and definition, the known inputs and outputs, the minimum and maximum number of samples, the dataset metadata (if any) and the list of possible goals.
   The answer must be one of the possible goals; otherwise the question is asked again, up to 3 times in total.

The list of possible goals depends on the model family and the hardware:

* **CNNs on the Axelera board**: when the model family (``model_family`` or ``type``) is ``CNNs`` and the hardware is ``Axelera Edge 130p``, the goals are the computer vision tasks of the YOLO models of the board:
  ``object-detection``, ``oriented-object-detection``, ``instance-segmentation``, ``semantic-segmentation`` and ``keypoint-detection``.
  They are read from the YOLO catalog of the WP2 module (``sustainml-wp2/hw_provider_axelera/yolo_models.json``), together with a description of each goal that is added to the prompt.
* **Any other case**: the ``Problem`` nodes of the Neo4j graph (see `External dependencies`_), minus a fixed list of goals that the framework does not support.
  With the current graph, the supported goals are ``summarization``, ``text-generation`` and ``translation``.

YOLO requirements
^^^^^^^^^^^^^^^^^

All the YOLO models of a goal do the same task on the same dataset; they differ only in how they trade accuracy for speed and energy.
For CNNs on the Axelera board, unless a model was chosen by hand, the node therefore also reads from the problem description what matters most when choosing among them, whether the goal is chosen in the :ref:`frontend` or inferred.
The node asks the ``llama3`` LLM whether the problem asks for accuracy, speed or energy, with a quote of the words of the problem that show each need,
and for the frame rate and the latency per image that the problem states, if any.
The LLM runs with temperature 0, so that every iteration of a problem reads the same requirements.
A need is only accepted when its quote is found in the problem and talks about that criterion.
The result is added to the ``extra_data`` of the output as ``yolo_preferences``:

.. code-block:: json

    {
        "priority": "speed",
        "min_fps": 120.0,
        "max_latency_ms": null,
        "reason": "speed: \"4 cameras at 30 fps\""
    }

* ``priority``: ``accuracy``, ``speed`` or ``energy`` when exactly one need is found, ``balanced`` otherwise.
* ``min_fps``: the total frames per second to process, only when the problem talks about a frame rate.
* ``max_latency_ms``: the maximum time per image in milliseconds, only when the problem states a time.
* ``reason``: the quotes that support the priority.

The :ref:`mlmodelprovider_node` uses these preferences to rank the YOLO models of the goal.

Configuration Requests
----------------------

The :ref:`orchestrator` routes to this node every :ref:`configuration request <node_config_service>` whose ``configuration`` string contains ``modality`` or ``dataset_path``.
The node answers the following requests:

+----------------------------+--------------------------------------------------------------+----------------------------------------------------------+
| Request ``configuration``  | Description                                                  | Reply ``configuration``                                  |
+============================+==============================================================+==========================================================+
| ``modality``               | List of the goals that the user can choose in the            | ``{"goals": "summarization, text-generation, ..."}``     |
|                            | :ref:`frontend`. The name of the request is kept for         |                                                          |
|                            | compatibility; the modalities are no longer used.            |                                                          |
+----------------------------+--------------------------------------------------------------+----------------------------------------------------------+
| ``dataset_path, <path>``   | Description of the dataset stored in ``<path>``, a ``.csv``  | ``{"description": "...", "profile": "...",``             |
|                            | or ``.json`` file readable by the node.                      | ``"topic": "...", "keywords": [...],``                   |
|                            |                                                              | ``"applications": [...]}``                               |
+----------------------------+--------------------------------------------------------------+----------------------------------------------------------+

* ``modality`` must be the whole ``configuration`` string.
  The reply lists, sorted and separated by ``", "``, the supported goals of the Neo4j graph (the YOLO goals of the Axelera board are not included; the :ref:`frontend` gets them from the :ref:`hwprovider_node`).
  ``success`` is ``false`` when the list is empty.
* ``dataset_path`` uses the *AutoDDG* package of the WP1 module: it takes a sample of 10 rows of the dataset, profiles its columns and asks the ``llama3`` LLM (through Ollama) for a semantic profile and then for a description of the dataset.
  The :ref:`frontend` sends the fields of the reply back in the ``dataset_metadata_*`` keys of the ``extra_data`` of the user input, so that they are added to the goal selection prompt.
  This request can take a long time, as it runs several LLM queries.
  ``success`` is ``false`` if the file cannot be read or described.

Any other request is answered with ``success`` set to ``false`` and ``{"error": "Unsupported configuration request: <request>"}``.

External dependencies
---------------------

* **Neo4j**: the knowledge graph of models and goals, reached at ``bolt://localhost:7687`` (user ``neo4j``, password ``12345678``) by the ``rdftool/rdfCode.py`` helpers of the WP1 module.
  The database is loaded from the dump files in ``sustainml-wp1/rag/neo4j_backup`` with ``neo4j-admin database load``, as explained in the :ref:`installation_framework`.
  At start-up, the node waits up to 5 seconds for the graph to answer, and exits if it does not.
* **Ollama**: the ``llama3`` model, served at ``http://localhost:11434``, for the goal selection, the YOLO requirements and the dataset description.
* **WP2 module**: the ``hw_provider_axelera`` package of ``sustainml-wp2``, for the YOLO goals and the normalization of the YOLO preferences.

Node Template
-------------

The following is the Python API of the :ref:`mlmodelmetadata_node`.
The user implements the functionality of the node in ``task_callback()``,
and the response to the configuration request from the orchestrator in ``configuration_callback()``.

The template below is a shortened version of ``sustainml-wp1/ml_model_metadata_node.py`` (`full file on GitHub <https://github.com/eProsima/sustainml-wp1/blob/main/ml_model_metadata_node.py>`__).
It keeps the structure of the node and the way it reads and writes its data. It leaves out, as described above:

* the goals of the YOLO models for CNNs on the Axelera board, and the ``yolo_preferences`` read from the description;
* the full prompt of the goal inference (with the problem definition, inputs, outputs, samples and dataset metadata, not only the short description) and its three attempts;
* the ``type`` and ``hardware_required`` keys copied into the ``extra_data``;
* the filter of the unsupported goals in the reply to ``modality``;
* the ``dataset_path`` request, which describes a dataset with AutoDDG and ``llama3``.

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
    """SustainML ML Model Metadata Node Implementation."""

    from sustainml_py.nodes.MLModelMetadataNode import MLModelMetadataNode

    # Manage signaling
    import signal
    import threading
    import time
    import json

    from rdftool.rdfCode import load_graph, get_problems

    from ollama import Client

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        MLModelMetadataNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: user_input
    # Outputs: node_status, ml_model_metadata
    def task_callback(user_input, node_status, ml_model_metadata):

        # The extra_data of the user input is a JSON object sent as a sequence of bytes
        extra_data_str = ''.join(chr(b) for b in user_input.extra_data())
        try:
            extra_data_dict = json.loads(extra_data_str)
        except json.JSONDecodeError:
            extra_data_dict = {}

        # Pass to the ML Model Provider node what it needs
        accumulated_data = {"problem_short_description": user_input.problem_short_description()}
        if "model_restrains" in extra_data_dict:
            accumulated_data["model_restrains"] = extra_data_dict["model_restrains"]
        if extra_data_dict.get("model_selected"):
            accumulated_data["model_selected"] = extra_data_dict["model_selected"]
        ml_model_metadata.extra_data(json.dumps(accumulated_data).encode("utf-8"))

        # Goal chosen in the GUI (or by the orchestrator when it reiterates)
        ui_goal = (extra_data_dict.get("goal") or "").strip()
        if ui_goal:
            ml_model_metadata.ml_model_metadata().append(ui_goal)
            return

        # A model chosen by hand needs no goal
        if extra_data_dict.get("model_selected"):
            return

        # Infer the goal with an LLM among the goals of the graph
        goals = [str(g) for g in get_problems()]
        client = Client(host='http://localhost:11434')
        prompt = (f"Given the following Information: \"{user_input.problem_short_description()}\". "
                  f"Which of the following machine learning Goals can be used to solve this problem: {goals}?. "
                  f"Answer with only one of the Machine learning goals and nothing more.")
        try:
            response = client.chat(model="llama3", messages=[{'role': 'user', 'content': prompt}])
            mlgoal = response['message']['content'].strip().lower()
        except Exception as e:
            print(f"Error in getting response from Ollama: {e}")
            mlgoal = None

        if mlgoal in goals:
            ml_model_metadata.ml_model_metadata().append(mlgoal)
        else:
            error_info = {"error": "Failed to extract metadata due to invalid or incomplete input parameters."}
            ml_model_metadata.extra_data(json.dumps(error_info).encode("utf-8"))

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())

        if req.configuration() == "modality":
            goals = ', '.join(sorted(str(g) for g in get_problems()))
            res.configuration(json.dumps(dict(goals=goals)))
            res.success(goals != "")
            res.err_code(0 if goals != "" else 1) # 0: No error || 1: Error
        else:
            error_msg = f"Unsupported configuration request: {req.configuration()}"
            res.configuration(json.dumps({"error": error_msg}))
            res.success(False)
            res.err_code(1) # 0: No error || 1: Error

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
            print("[Error][ml_model_metadata] Graph not available")
            exit(1)
        node = MLModelMetadataNode(callback=task_callback, service_callback=configuration_callback)
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
