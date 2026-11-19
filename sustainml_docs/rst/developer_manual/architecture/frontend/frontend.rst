.. _frontend:

Front-End Node
==============

.. raw:: html

   <style>
        .module_node:not(#front-end) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#front-end) {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../../figures/sustainml_framework_arch.svg

The Front-End component within the software architecture of the *SustainML Framework* is the user-facing software component.
It is responsible for displaying information, receiving user input for defining a new task, and providing a
means for users to interact with the underlying functionality of the *SustainML Framework* in synchronization with the :ref:`orchestrator`.

The Front-End is the ``sustainml`` desktop application of the ``sustainml_framework`` repository, written in C++ and
QML with Qt 5.15.
It does not join the DDS domain: it exchanges all its data with the Back-End through the REST API that the Back-End
serves on ``http://127.0.0.1:5001``, and only uses ``sustainml_cpp`` for its ``TaskId`` and ``NodeID`` types.

Application structure
---------------------

* ``src/main.cpp`` creates the Qt application, registers the QML singletons (``Settings``, ``SustainMLFont``,
  ``ScreenManager`` and ``Theme``) and the ``SustainMLTreeModel`` type, loads the fonts, and creates the ``Engine``.
* ``Engine`` (``include/sustainml/Engine.hpp`` and ``src/cpp/Engine.cpp``) is a ``QQmlApplicationEngine`` that loads
  ``qrc:/qml/main.qml`` and exposes itself to QML as the ``engine`` context property.
  The QML code calls its slots and ``Q_INVOKABLE`` methods (``launch_task()``, ``request_hf_models()``,
  ``save_all()``, ...), and the ``Engine`` sends the results back to QML through its signals
  (``new_ml_model_node_output()``, ``hf_models_available()``, ``saved_files_available()``, ...).
* ``REST_requester`` (``include/sustainml/REST_requester.hpp`` and ``src/cpp/REST_requester.cpp``) performs one REST
  request: it sends a ``POST`` with a JSON body to ``http://127.0.0.1:5001`` plus the route of its request type, and
  calls a callback with the JSON object of the reply.
  The ``Engine`` creates a requester per request, keeps it in a list while it waits for the reply, and deletes it once
  the callback has run, so all the requests are asynchronous.
* ``src/qml`` holds the user interface: ``main.qml``, the screens (``SmlHomeScreen``, ``SmlProblemDefinitionScreen``,
  ``SmlLoadDatasetScreen``, ``SmlResultsScreen``, ``SmlYoloModelsScreen`` and ``SmlSettingsScreen``), the fragments of
  the results screen, the reusable ``Sml*`` components, and the ``utils`` folder (theme, screen manager, list of
  countries and U-Net model structure).

How the Front-End uses the Back-End
-----------------------------------

* A timer asks for the status of the nodes (``/status``) every 500 ms.
  The first time the :ref:`hwprovider_node` reports ``IDLE``, the ``Engine`` asks it for the list of hardware
  (``hardwares`` configuration request), and the first time the :ref:`mlmodelmetadata_node` reports ``IDLE``, it asks
  it for the list of ML goals (``modality`` configuration request).
  When the six nodes are ``IDLE``, the application leaves its initializing state.
* ``launch_task()`` sends the problem defined by the user to ``/user_input``.
  With the ``task_id`` of the reply, the ``Engine`` asks ``/results`` for the result of each of the six nodes for that
  task, one request per node.
  The Back-End answers each request when the output of that node has arrived, so the results appear in the Front-End
  node by node.
* When the result of the :ref:`carbontracker_node` says that another iteration follows (``auto_reiterate`` set, or
  ``num_outputs`` greater than ``1`` in its ``extra_data``), the ``Engine`` asks for the results of the next
  iteration of the problem, which the :ref:`orchestrator` launches on its own.
  Otherwise, it signals the end of the task.
  The cancel action of the Front-End only stops following the iterations: the Back-End has no cancel request.
* The lists of goals, hardware, models of a goal, YOLO models of the Axelera board and country carbon intensities, the
  Hugging Face searches and the dataset description are obtained with configuration requests (``/config_request``),
  described in :ref:`node_config_service`.
  The ``Engine`` adds a unique ``request_id`` to each of them and keeps its callback under that ``request_id``, which
  the Back-End returns in the reply.
* The Hugging Face model information and comparison use ``/hf_models_info`` and ``/hf_models_compare``, and the save
  files use the ``/saved_files``, ``/save_*``, ``/load_*``, ``/delete_saved_file`` and ``/wipe_db`` endpoints.
  Loaded tasks are shown by asking ``/results`` for them, exactly as for a live task.

REST API of the Back-End
------------------------

The Back-End (``sustainml-wp5/backend_node.py``) is a Flask server on ``http://127.0.0.1:5001`` that serves several
requests at a time.
The Front-End sends every request as a ``POST`` with a JSON body (an empty object when there is nothing to send), even
for the endpoints that also accept ``GET``.
The ``/`` and ``/terminate`` endpoints are not used by the Front-End.

.. list-table::
   :header-rows: 1

   * - Endpoint
     - Method
     - Purpose
   * - ``/``
     - ``GET``
     - Check that the Back-End is running.
   * - ``/user_input``
     - ``POST``
     - Launch a task (a new problem or a new iteration).
   * - ``/config_request``
     - ``POST``
     - Send a configuration request to a module node.
   * - ``/hf_models_info``
     - ``POST``
     - Get the Hugging Face information of some models.
   * - ``/hf_models_compare``
     - ``POST``
     - Compare up to five Hugging Face models with an LLM.
   * - ``/status``
     - ``GET``, ``POST``
     - Get the status of the nodes.
   * - ``/results``
     - ``GET``, ``POST``
     - Get the results of a task.
   * - ``/saved_files``
     - ``GET``, ``POST``
     - List the save files.
   * - ``/save_tasks``, ``/load_tasks``
     - ``POST``
     - Save tasks into a save file, or load them from it.
   * - ``/save_all``, ``/load_all``
     - ``POST``
     - Save tasks and the Hugging Face history into a save file, or load them from it.
   * - ``/save_hf``, ``/load_hf``
     - ``POST``
     - Save one part of the Hugging Face history into a save file, or load it from it.
   * - ``/delete_saved_file``
     - ``POST``
     - Delete a save file.
   * - ``/wipe_db``
     - ``POST``
     - Delete every save file.
   * - ``/terminate``
     - ``GET``
     - Stop the Back-End.

All the replies are JSON objects, except the ``GET /results`` reply when no task has been launched yet.
The endpoints that need a field (for instance, the ``name`` of the save file endpoints) reply with status ``400`` and
an ``error`` field when it is missing.
The save files are explained in :ref:`orchestrator`.

``/``
^^^^^

* Request: ``GET``, no body.
* Reply: ``{"message": "Hello world! Use \"/terminate\" route to stop Back-end node."}``.

``/user_input``
^^^^^^^^^^^^^^^

* Request: the user input of the task, with the fields of :ref:`user_input_type` (``task_id`` excluded).
  The Front-End sends ``problem_short_description``, ``problem_definition``, ``inputs`` and ``outputs`` (lists of
  words), ``minimum_samples``, ``maximum_samples``, ``optimize_carbon_footprint_auto``,
  ``optimize_carbon_footprint_manual``, ``previous_iteration`` (``0`` for a new problem), ``desired_carbon_footprint``,
  ``geo_location_continent``, ``geo_location_region`` (a country code) and an ``extra_data`` object with
  ``hardware_required``, ``max_memory_footprint``, ``goal``, ``previous_problem_id``, ``num_outputs``,
  ``model_selected``, ``type`` (the model family), the ``dataset_metadata_*`` fields, and ``model_restrains`` and
  ``hf_analyze`` when they are set.
* The Back-End adds to ``extra_data`` the Hugging Face token of its ``HF_TOKEN`` environment variable (``hf_token``)
  and the ``model_family``: the ``model_family`` of ``extra_data``, else the ``type`` of the request or of
  ``extra_data``, else ``Transformers``.
  Then the :ref:`orchestrator` launches the task.
* Reply: ``{"message": "User input data sent successfully.", "task_id": {"problem_id": <int>, "iteration_id": <int>}}``.

``/config_request``
^^^^^^^^^^^^^^^^^^^

* Request: ``{"configuration": <request string>, "node_id": <int>, "request_id": <int>}``.
  The orchestrator chooses the target node from keywords of ``configuration``, not from ``node_id``
  (see :ref:`node_config_service`).
* Reply, always with status ``200``: ``{"message": "Configuration request sent successfully.", "response":
  {"node_id": <int>, "transaction_id": <int>, "success": <bool>, "err_code": <int>, "configuration": <string>,
  "request_id": <int>}}``, where ``request_id`` is the one of the request (``0`` if it had none) and ``configuration``
  is the answer of the node, usually a JSON object encoded as a string.

``/hf_models_info``
^^^^^^^^^^^^^^^^^^^

* Request: ``{"model_ids": [<Hugging Face model id>, ...]}``.
* The Back-End asks the Hugging Face API (``https://huggingface.co/api/models/<id>`` with ``cardData`` and ``config``)
  for every model, with the token of its ``HF_TOKEN`` environment variable.
* Reply: ``{"models": [{"model_id", "status", "info", "error"}, ...], "errors": [{"model_id", "error"}, ...]}``, where
  ``status`` is the HTTP status of the Hugging Face reply (``0`` if the request failed), ``info`` its JSON body, and
  ``error`` is only present for the models that failed.

``/hf_models_compare``
^^^^^^^^^^^^^^^^^^^^^^

* Request: ``{"models": [...]}``, with items of the ``models`` list of the ``/hf_models_info`` reply.
  Only the first five are compared.
* The Back-End gathers the license, model family, architectures and languages of each model, and asks the ``llama3``
  model of the local Ollama server for a comparison based only on those facts.
* Reply: ``{"comparison_summary": <string>, "model_cards": [{"model_id", "purpose", "best_for", "watch_out_for"}, ...],
  "comparison_table": [{"model_id", "license", "model_family", "architectures", "languages"}, ...]}``, with one card
  and one table row per model.
  If the LLM fails or does not answer with JSON, the reply has status ``500`` and an ``error`` field.

``/status``
^^^^^^^^^^^

* Request: ``GET``, or ``POST`` with an optional ``node_id`` (see :ref:`node_ids`).
* Reply: ``{"status": {<node name>: <status>, ...}}`` with every node that has reported its status, or
  ``{"status": <status>}`` when ``node_id`` is given (``INACTIVE`` if that node has not reported yet).
  The node names are ``APP_REQUIREMENTS``, ``CARBON_FOOTPRINT``, ``HW_CONSTRAINTS``, ``HW_RESOURCES``,
  ``ML_MODEL_METADATA`` and ``ML_MODEL``, and the statuses are explained in :ref:`orchestrator`.

``/results``
^^^^^^^^^^^^

* Request: ``GET``, for the results of all the nodes for the last task, or ``POST`` with
  ``{"node_id": <int>, "task_id": {"problem_id": <int>, "iteration_id": <int>}}``.
  ``node_id`` is one of the :ref:`node_ids`, ``7`` for the user input of the task, or ``9`` for the results of the six
  nodes.
  Without ``task_id``, the last task is used, but ``task_id`` is required with ``node_id`` ``9``.
* The reply waits until the requested node results are available.
* Reply: ``{<node name>: <result>}`` for one node (``ORCHESTRATOR`` for the user input), or an object with the result
  of each of the six nodes and the ``task_id`` for ``GET`` and ``node_id`` ``9``.
  The ``GET`` reply is the text ``Nodes have not reported any output yet.`` if no task has been launched.
  A ``task_id`` with negative ids gets a ``400`` reply, and an unknown one a ``404`` reply.
* Every result has the ``task_id`` it belongs to and the fields of the node output (see :ref:`data_model`), with the
  sequences of strings joined in a single string:

  * ``APP_REQUIREMENTS``: ``app_requirements``.
  * ``ML_MODEL_METADATA``: ``keywords`` and ``metadata`` (the ML goal).
  * ``HW_CONSTRAINTS``: ``max_memory_footprint`` and ``hardware_required``.
  * ``ML_MODEL``: ``model``, ``model_path``, ``model_properties``, ``model_properties_path``, ``input_batch`` and
    ``target_latency``.
  * ``HW_RESOURCES``: ``hw_description``, ``power_consumption``, ``latency`` and ``memory_footprint_of_ml_model``.
  * ``CARBON_FOOTPRINT``: ``carbon_footprint``, ``energy_consumption``, ``carbon_intensity`` and ``extra_data`` (a
    JSON object).
  * ``ORCHESTRATOR``: all the fields of the user input, with ``inputs`` and ``outputs`` joined with spaces and
    ``extra_data`` as a JSON object.

  For a task loaded from a save file, a node result that was not saved is ``{"message": ..., "task_id": ...}``.

``/saved_files``
^^^^^^^^^^^^^^^^

* Request: ``GET``, or ``POST`` with an optional ``part``: ``tasks``, ``searches`` or ``comparisons``.
* Reply: ``{"names": [<save name>, ...]}``, sorted, with only the files that hold data of ``part`` when it is given.

``/save_tasks``
^^^^^^^^^^^^^^^

* Request: ``{"name": <save name>, "tasks": [{"problem_id", "iteration_id", "display_name"}, ...]}``, with at least
  one task.
  The Front-End sends every task of the session and the name the user gave to each problem.
* The tasks replace the tasks of the save file, which is created if it does not exist.
* Reply: ``{"message": "Saved <n> task(s) to \"<name>\".", "path": <absolute path of the save file>}``.

``/load_tasks``
^^^^^^^^^^^^^^^

* Request: ``{"name": <save name>}``.
* Reply: ``{"tasks": [{"problem_id", "iteration_id", "display_name"}, ...]}``, with the new ``problem_id`` given to
  each loaded problem (an empty list if the file does not exist).
  The Front-End then asks ``/results`` for each loaded task.

``/save_all``
^^^^^^^^^^^^^

* Request: ``{"name": <save name>, "tasks": [...], "hf_searches": [...], "hf_comparisons": [...]}``, with ``tasks`` as
  in ``/save_tasks`` (it can be empty here).
* Saves the tasks and both parts of the Hugging Face history into the save file.
* Reply: ``{"message": "Saved everything to \"<name>\".", "path": <absolute path of the save file>}``.

``/load_all``
^^^^^^^^^^^^^

* Request: ``{"name": <save name>}``.
* Reply: ``{"tasks": [...], "hf_searches": [...], "hf_comparisons": [...]}``, with ``tasks`` as in ``/load_tasks``.

``/save_hf``
^^^^^^^^^^^^

* Request: ``{"name": <save name>, "part": "searches" | "comparisons", "data": [...]}``.
* Replaces that part of the Hugging Face history of the save file, keeping its tasks and the other part.
* Reply: ``{"message": "Saved <part> to \"<name>\".", "path": <absolute path of the save file>}``.

``/load_hf``
^^^^^^^^^^^^

* Request: ``{"name": <save name>, "part": "searches" | "comparisons"}``.
* Reply: ``{"part": <part>, "data": [...]}``.

``/delete_saved_file``
^^^^^^^^^^^^^^^^^^^^^^

* Request: ``{"name": <save name>}``.
* Deletes that save file, if it exists.
* Reply: ``{"message": "Deleted \"<name>\"."}``.

``/wipe_db``
^^^^^^^^^^^^

* Request: no fields.
* Deletes every save file of the save directory.
  The tasks of the session are not affected.
* Reply: ``{"message": "All save files deleted."}``.

``/terminate``
^^^^^^^^^^^^^^

* Request: ``GET``, no body.
* Stops the :ref:`orchestrator` and the Flask server, which ends the Back-End process.
* Reply: ``{"message": "Terminating...<br>"}``.

.. _node_ids:

Node identifiers
----------------

The ``node_id`` values used by the REST API and by the configuration requests are the values of the ``NodeID`` enum of
``sustainml_cpp`` (``sustainml_cpp/include/sustainml_cpp/core/Constants.hpp``):

.. list-table::
   :header-rows: 1

   * - ``node_id``
     - Node
     - Name in the REST replies
   * - ``0``
     - :ref:`appreqs_node`
     - ``APP_REQUIREMENTS``
   * - ``1``
     - :ref:`carbontracker_node`
     - ``CARBON_FOOTPRINT``
   * - ``2``
     - :ref:`hwconstraints_node`
     - ``HW_CONSTRAINTS``
   * - ``3``
     - :ref:`hwprovider_node`
     - ``HW_RESOURCES``
   * - ``4``
     - :ref:`mlmodelmetadata_node`
     - ``ML_MODEL_METADATA``
   * - ``5``
     - :ref:`mlmodelprovider_node`
     - ``ML_MODEL``
   * - ``7``
     - :ref:`orchestrator` (user input of a task)
     - ``ORCHESTRATOR``
