.. _node_config_service:

Node Configuration Service
--------------------------

This service lets the Front-End ask any module node for information at run time, while the node keeps processing
tasks.
The Front-End uses it to fill its lists (ML goals, hardware, models of a goal, YOLO models of the Axelera board and
country carbon intensities), to search Hugging Face models and to describe a dataset.
Although the remote method is called ``update_configuration()``, no node changes its configuration with it in the
current implementation: every request is a query, answered from the data of the node.
The data types of the service are described in :ref:`Service Types <service_types>`.

Request path
^^^^^^^^^^^^

A configuration request goes through the following steps:

#. The Front-End sends ``{"configuration": <request string>, "node_id": <int>, "request_id": <int>}`` to the
   ``/config_request`` endpoint of the Back-End (see :ref:`frontend`).
#. The Python ``Orchestrator.send_request()`` (``sustainml-wp5/orchestrator_node/orchestrator_node.py``) builds a
   ``RequestType`` with a new ``transaction_id`` (a counter of the orchestrator) and the request string as
   ``configuration``, and chooses the target node from keywords of the request string (see
   :ref:`config_request_routing`).
   The ``node_id`` sent by the Front-End is ignored.
#. ``OrchestratorNode::configuration_request()`` (C++) calls the ``update_configuration()`` remote method of the
   service of the node given in the ``node_id`` of the ``RequestType``, and blocks until the node replies.
#. In the node, ``GenericServiceNodeImpl::update_configuration()`` (``sustainml_cpp/src/cpp/core``) builds a
   ``RequestType`` that only carries the ``configuration`` string, and passes it, with an empty ``ResponseType``, to
   the ``on_configuration_request()`` method of the node listener, which calls the configuration callback of the
   Python node (``configuration_callback(req, res)`` in the module nodes).
#. The callback fills ``success``, ``err_code`` and ``configuration`` in the ``ResponseType``, and
   ``GenericServiceNodeImpl`` encodes them in the string that the remote method returns (see
   :ref:`config_request_reply`).
#. The orchestrator decodes that string into a ``ResponseType``, with the ``node_id`` and ``transaction_id`` of the
   request, and the Back-End returns it to the Front-End together with the ``request_id``.

The node never sees the ``node_id`` and ``transaction_id`` of the request (they are ``0`` in the ``RequestType`` it
receives), and the values it writes in those fields of the ``ResponseType`` are not used.

RPC over DDS
^^^^^^^^^^^^

The service is built on the RPC over DDS of Fast DDS.
``SustainMLService.idl`` (``sustainml_cpp/include/sustainml_cpp/idl``) defines one interface per module node, all of
them with the same method:

.. code-block:: omg-idl

    exception InternalError
    {
    };

    interface AppRequirementsService
    {
        string update_configuration(in string configuration) raises (InternalError);
    };

    // The same for HWConstraintsService, HWResourcesService, CarbonFootprintService,
    // MLModelMetadataService and MLModelService

Each module node creates the server of its interface, with the interface name as service name
(``AppRequirementsService``, ``HWConstraintsService``, ``HWResourcesService``, ``CarbonFootprintService``,
``MLModelMetadataService`` or ``MLModelService``), and runs it in a background thread with a single worker thread:
a node answers its configuration requests one at a time, while it keeps processing tasks.
The orchestrator creates one client per interface.

``OrchestratorNode::configuration_request()`` waits for the reply at most the number of minutes of the
``SUSTAINML_RPC_TIMEOUT_MINUTES`` environment variable (``10`` by default), and stops waiting if the orchestrator is
terminated.
The request fails, with ``success`` set to ``false``, ``err_code`` ``0`` (``NO_ERROR``) and an empty
``configuration``, when:

* The reply does not arrive in time, or the orchestrator is terminating.
* The node raises ``InternalError``, which it does when it is shutting down or when its callback throws an exception.
* Any other RPC error occurs, or the ``node_id`` is not one of the six module nodes.

.. _config_request_reply:

Reply encoding
^^^^^^^^^^^^^^

The remote method returns a single string, which has no room for the success flag and the error code that the node
sets in its ``ResponseType``.
``GenericServiceNodeImpl`` therefore encodes all three in the returned string with ``encode_configuration_reply()``
(``sustainml_cpp/src/cpp/common/Common.hpp``):

.. code-block:: text

    <success>;<err_code>;<configuration>

where ``<success>`` is ``1`` or ``0``, ``<err_code>`` is the error code in decimal digits (``0`` for ``NO_ERROR``,
``1`` for ``INTERNAL_ERROR``), and ``<configuration>`` is the configuration string of the node, which can contain any
character, ``;`` included.
For instance, ``0;1;{"error": "Unsupported configuration request: foo"}``.

``OrchestratorNode::configuration_request()`` splits it again with ``decode_configuration_reply()`` and fills
``success``, ``err_code`` and ``configuration`` of the ``ResponseType`` with the values of the node.
A string that does not follow this format (a single ``0`` or ``1``, ``;``, from one to nine digits and ``;``) is taken
as a bare configuration: ``success`` is ``true`` and the whole string is the ``configuration``.

.. _config_request_routing:

Routing of the requests
^^^^^^^^^^^^^^^^^^^^^^^

``Orchestrator.send_request()`` sets the ``node_id`` of the request by looking for these keywords anywhere in the
request string, in this order:

.. list-table::
   :header-rows: 1

   * - Keywords
     - Target node
     - ``node_id``
   * - ``hardwares``, ``yolo_models``
     - :ref:`hwprovider_node`
     - ``3``
   * - ``carbon_intensities``
     - :ref:`carbontracker_node`
     - ``1``
   * - ``modality``, ``dataset_path``
     - :ref:`mlmodelmetadata_node`
     - ``4``
   * - ``model_from_goal``, ``hf_search``
     - :ref:`mlmodelprovider_node`
     - ``5``
   * - Any other string
     - :ref:`appreqs_node`
     - ``0`` (the default value)

A request string with none of the keywords goes to the :ref:`appreqs_node`, which answers every request with the
placeholder ``{"param1": "value1", "param2": "value2", "param3": "value3"}`` and ``success`` set to ``true``.
Therefore, a new request must have its keyword added to ``send_request()``; an answer with ``param1`` means that the
request did not match any keyword.
Since the keywords are looked for anywhere in the string, a request whose free text (for instance, the description
of a Hugging Face search) contains one of the keywords of an earlier row is sent to the node of that row.
No request reaches the :ref:`hwconstraints_node`, which would answer any of them as unsupported.

Requests
^^^^^^^^

The following table lists every request that the nodes answer.
Unless stated otherwise, the reply ``configuration`` is a JSON object encoded as a string, and ``err_code`` is ``0``
when ``success`` is ``true`` and ``1`` when it is ``false``.

.. list-table::
   :header-rows: 1
   :widths: 20 15 30 35

   * - Request string
     - Node
     - Reply ``configuration``
     - Used by the Front-End
   * - ``hardwares``
     - :ref:`hwprovider_node`
     - ``{"hardwares": "<name>, <name>, ..."}``: the architectures of the UPMEM simulator, the Axelera devices and
       ``FPGA (xczu19eg-ffvb1517-2-i)``.
       On error, ``success`` is ``false`` and ``configuration`` is empty.
     - ``Engine::request_hardwares()``, the first time the node is ``IDLE``, to fill the hardware list.
   * - ``yolo_models``
     - :ref:`hwprovider_node`
     - ``{"yolo_models": [...]}``: one object per YOLO model of the Axelera board, with ``name``, ``display_name``,
       ``architecture``, ``size``, ``goal``, ``metric``, ``accuracy_fp32``, ``accuracy_int8``, ``latency_s``,
       ``power_w``, ``energy_per_inference_j``, ``memory_mb`` and ``power_source``.
       On error, ``{"error": <message>}``.
     - ``Engine::request_yolo_models()``, for the YOLO models screen.
   * - ``carbon_intensities``
     - :ref:`carbontracker_node`
     - ``{"carbon_intensities": {<ISO country code>: {"country", "year", "g_per_kwh"}, ...}}``: the grid carbon
       intensity of every country of ``carbon_intensities.csv``.
       On error, ``{"error": "carbon_intensities.csv could not be read"}``.
     - ``Engine::request_carbon_intensities()``, for the screens that turn energy into CO2 for a chosen country.
   * - ``modality``
     - :ref:`mlmodelmetadata_node`
     - ``{"goals": "<goal>, <goal>, ..."}``: the ML goals of the knowledge graph, without the unsupported ones,
       sorted.
       ``success`` is ``false`` if the list is empty; on error, ``configuration`` is empty.
       The name of the request is kept from when it returned the modalities, which are no longer used.
     - ``Engine::request_goals()``, the first time the node is ``IDLE``, to fill the goal list.
   * - ``dataset_path, <path>``
     - :ref:`mlmodelmetadata_node`
     - ``{"description", "profile", "topic", "keywords", "applications"}``: the description of the ``.csv`` or
       ``.json`` dataset file, generated with the ``llama3`` model of Ollama.
       On error, ``configuration`` is empty.
       It can take minutes.
     - ``Engine::launch_dataset_path_task()``, from the dataset screen.
   * - ``model_from_goal, <goal>[, <hardware>[, <family>]]``
     - :ref:`mlmodelprovider_node`
     - ``{"models": "<name>, <name>, ..."}``: with family ``CNNs`` and an Axelera device, the YOLO models of the goal;
       with goal ``U_NET_MODELS``, or family ``CNNs`` and an FPGA, the U-Net models shipped with the node; otherwise,
       the models of the goal in the knowledge graph.
       ``success`` is ``false`` when the list of YOLO or graph models is empty, and on error
       (``{"models": ""}``).
     - ``Engine::request_model_from_goal()``, to fill the model list.
       The Front-End sends ``<goal>``, ``<goal>, <family>``, ``<goal>, <hardware>, <family>`` (YOLO models) or
       ``U_NET_MODELS, <hardware>, CNNs``.
   * - ``hf_search, <description>, <limit>``
     - :ref:`mlmodelprovider_node`
     - The reply of the ``hf_search_models`` tool of the Hugging Face MCP server of the node: ``{"models": [...],
       "task", "pipeline_tag", "candidates"}``, with up to ``<limit>`` models (``20`` if it is missing), each one with
       its ``model_id`` and a ``tooltip`` among other Hugging Face fields, or ``{"models": [], "error": <message>}``.
       The node tries three times; if all fail, ``success`` is ``false`` and the reply is
       ``{"models": [], "error": <message>}``.
     - ``Engine::request_hf_models()``, with a limit of ``10``, for the Hugging Face search.
   * - ``hf_search, __MODEL_CONFIG__:<model id>, 1``
     - :ref:`mlmodelprovider_node`
     - ``{"model_id", "tooltip"}``: a summary of the ``config.json`` of the model, plus ``error`` if it could not be
       read (for instance, for gated models).
     - ``Engine::request_hf_model_tooltip()``, for the tooltip of a model of the Hugging Face search.

Any other request that reaches the :ref:`mlmodelmetadata_node`, the :ref:`hwprovider_node` or the
:ref:`carbontracker_node` is answered with ``success`` set to ``false``, ``err_code`` ``1`` and
``{"error": "Unsupported configuration request: <request string>"}``; the :ref:`mlmodelprovider_node` answers it with
``{"models": ""}``.
The ``hardwares``, ``yolo_models``, ``carbon_intensities`` and ``modality`` requests must match exactly, while the
other ones only need to start with (``model_from_goal`` and ``hf_search``) or contain (``dataset_path``) their name.
