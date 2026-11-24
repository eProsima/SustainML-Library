.. _architecture:

SustainML Framework Architecture
================================

.. raw:: html

   <style>
        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../figures/sustainml_framework_arch.svg

The *SustainML Framework* takes the description of a machine learning problem given by the user, proposes an ML model
to solve it, estimates the resources (latency, power consumption and memory footprint) that model needs on the hardware
selected by the user, and estimates the energy consumption and the carbon footprint of running it.

Within the *SustainML Architecture*, a ``Node`` refers to each one of the software modules that performs
a particular task, conforms a single executable unit and can be locally or remotely deployed.
The nodes communicate through the `eProsima Fast DDS middleware <https://www.eprosima.com/index.php/products-all/eprosima-fast-dds>`_,
which provides this decoupling and modularity.
All of them join the DDS domain ``0``, or the one set in the ``SUSTAINML_DOMAIN_ID`` environment variable.

The framework is made of three parts:

* The ``Front-End``, the interface between the user and the *SustainML Framework*.
  It is a Qt5/QML desktop application (the ``sustainml`` executable of the ``sustainml_framework`` repository) that
  collects the user requirements and constraints, sends them to the Back-End and shows the intermediate and final
  results.
  It does not take part in the DDS communication: it talks to the Back-End through a REST API.

  * :ref:`frontend`

* The ``Back-End`` (``sustainml-wp5/backend_node.py``), a Python process that serves the REST API used by the
  Front-End (a Flask server on ``http://127.0.0.1:5001``) and runs the :ref:`orchestrator`, which controls the
  execution of the tasks and gathers the data of the nodes for the Front-End.
* The :ref:`module_nodes`, six Python processes that specialize in particular tasks of the solution.

The software is split in the following packages of the ``sustainml_lib`` repository:

* ``sustainml_cpp``: the C++ library with the DDS types (see :ref:`data_model`), the base class of each module node,
  the ``OrchestratorNode`` and the configuration service (see :ref:`node_config_service`).
* ``sustainml_swig``: the SWIG Python bindings of ``sustainml_cpp``.
* ``sustainml_py``: the Python API of the module nodes, on top of ``sustainml_swig``.
  Each module node only implements a callback that receives the inputs of a task and fills its output, and optionally a
  callback that answers configuration requests; the library hides the DDS communication.
* ``sustainml_modules``: the nodes themselves, grouped by the work package of the project that developed them
  (``sustainml-wp1`` to ``sustainml-wp5``).
  The ``sustainml-wp4`` folder holds a web front end that the current framework does not use.

Some nodes also rely on external services that run on the same machine: a `Neo4j <https://neo4j.com/>`_ database with
the knowledge graph of ML goals and models (``bolt://localhost:7687``), an `Ollama <https://ollama.com/>`_ server with
the ``llama3`` model (``http://localhost:11434``), and the Hugging Face Hub, accessed with the token of the ``HF_TOKEN``
environment variable.

The ``sustainml-framework`` script (``framework_run.sh`` in the ``sustainml_framework`` repository) starts Neo4j, the
Back-End, the six module nodes and the Front-End, in that order, and stops all of them when the Front-End is closed.
The Back-End is started first, so that the :ref:`orchestrator` is already listening when the nodes report their first
status.

Data flow
"""""""""

When the user launches a task, the :ref:`orchestrator` publishes the user input, and every node publishes its output
in its own topic, which the following nodes and the orchestrator subscribe to:

.. list-table::
   :header-rows: 1

   * - Topic
     - Type
     - Published by
     - Subscribed by
   * - ``/sustainml/user_input``
     - :ref:`UserInputImpl <user_input_type>`
     - :ref:`orchestrator`
     - :ref:`appreqs_node`, :ref:`mlmodelmetadata_node`, :ref:`hwconstraints_node`, :ref:`carbontracker_node`
   * - ``/sustainml/app_requirements/output``
     - :ref:`AppRequirementsImpl <apprequirements_type>`
     - :ref:`appreqs_node`
     - :ref:`mlmodelprovider_node`, :ref:`hwprovider_node`, :ref:`orchestrator`
   * - ``/sustainml/ml_model_metadata/output``
     - :ref:`MLModelMetadataImpl <mlmodelmetadata_type>`
     - :ref:`mlmodelmetadata_node`
     - :ref:`mlmodelprovider_node`, :ref:`orchestrator`
   * - ``/sustainml/hw_constraints/output``
     - :ref:`HWConstraintsImpl <hardware_constraints_type>`
     - :ref:`hwconstraints_node`
     - :ref:`mlmodelprovider_node`, :ref:`hwprovider_node`, :ref:`orchestrator`
   * - ``/sustainml/ml_model_provider/output``
     - :ref:`MLModelImpl <mlmodel_type>`
     - :ref:`mlmodelprovider_node`
     - :ref:`hwprovider_node`, :ref:`carbontracker_node`, :ref:`orchestrator`
   * - ``/sustainml/hw_resources/output``
     - :ref:`HWResourceImpl <hardware_resource_type>`
     - :ref:`hwprovider_node`
     - :ref:`carbontracker_node`, :ref:`orchestrator`
   * - ``/sustainml/carbon_tracker/output``
     - :ref:`CO2FootprintImpl <carbonfootprint_type>`
     - :ref:`carbontracker_node`
     - :ref:`orchestrator`
   * - ``/sustainml/ml_model_provider/baseline``,
       ``/sustainml/hw_resources/baseline``,
       ``/sustainml/carbon_tracker/baseline``
     - ``MLModelImpl``, ``HWResourceImpl``, ``CO2FootprintImpl``
     - :ref:`orchestrator`
     - :ref:`mlmodelprovider_node`
   * - ``/sustainml/status``
     - :ref:`NodeStatusImpl <node_status_type>`
     - Every module node
     - :ref:`orchestrator`
   * - ``/sustainml/control``
     - :ref:`NodeControlImpl <node_control_type>`
     - :ref:`orchestrator`
     - Every module node

When the orchestrator starts a task, it also publishes on the baseline topics the results of the previous iteration of
the same problem (empty data in the first iteration of a problem).
The :ref:`mlmodelprovider_node` waits for these three baselines together with its other three inputs before running a
task.

Besides these topics, every module node serves a Fast DDS RPC service through which the orchestrator sends it
configuration requests, as explained in :ref:`node_config_service`.

.. _module_nodes:

Module Nodes
""""""""""""

The ``Module Nodes`` that specialize in a particular task of the solution are:

* :ref:`appreqs_node`
* :ref:`carbontracker_node`
* :ref:`hwconstraints_node`
* :ref:`mlmodelmetadata_node`
* :ref:`mlmodelprovider_node`
* :ref:`hwprovider_node`


.. toctree::
   :maxdepth: 1
   :hidden:

   frontend/frontend
   backend/orchestrator
   backend/module_nodes/apprequirements
   backend/module_nodes/carbontracker
   backend/module_nodes/hwconstraints
   backend/module_nodes/mlmodelmetadata
   backend/module_nodes/mlmodelprovider
   backend/module_nodes/hwprovider
