.. _data_model:

SustainML Framework Data Model
==============================

Following are the Data Structures definitions using the `Interface Definition Language specification <https://www.omg.org/spec/IDL/4.2/About-IDL>`_.

Every node in the *SustainML Framework* needs to provide a continuous feedback status to the :ref:`orchestrator`.
This is modeled with the ``NodeStatus`` data structure.

.. _node_status_type:

Node Status Type
----------------

.. code-block:: bash

    #NodeStatus.idl

    enum Status {
            INACTIVE,
            INITIALIZING,
            IDLE,
            RUNNING,
            ERROR,
            TERMINATING};

    enum TaskStatus {
            WAITING,
            RUNNING,
            ERROR,
            SUCCEEDED};

    enum ErrorCode {
            NO_ERROR,
            INTERNAL_ERROR};

    struct NodeStatus
    {

        Status node_status;
        TaskStatus task_status;
        ErrorCode error_code;
        @key long task_id;
        string error_description;
        @key string node_name;
    };

.. _node_control_type:

Node Control Type
-----------------

In order to correctly manage the lifecycle of nodes and tasks, a ``NodeControl`` data structure is defined.
This data structure is internally used in the communication library.

.. code-block:: bash

    #NodeControl.idl

    enum CmdNode {
        NO_CMD,
        CMD_START_NODE,
        CMD_STOP_NODE,
        CMD_RESET_NODE,
        CMD_TERMINATE_NODE};

    enum CmdTask {
        NO_CMD,
        CMD_STOP_TASK,
        CMD_RESET_TASK,
        CMD_PREEMPT_TASK,
        CMD_TERMINATE_TASK};

    struct NodeControl
    {

        CmdNode cmd_node;
        CmdTask cmd_task;
        string target_node;
        long task_id;
        @key string source_node;
    };

.. _user_input_type:

User Input Type
---------------

The ``UserInput`` data structure carries information about the input from the user, when describing a new task (Machine Learning problem).
It is comprised of the following fields:

* ``task_name``: The name of the task.
* ``modality``: The modality of the input data e.g image/video, text, audio, sensor,...
* ``problem_definition``: The type of problem to solve e.g classification, regression, clustering,...
* ``inputs``: A sequence of serialized batches of input data.
* ``outputs``: A sequence of serialized batches of output data.
* ``minimum_samples``: The minimum number of samples required.
* ``maximum_samples``: The maximum number of samples required.
* ``optimize_carbon_footprint_manual``: A boolean indicating if the user wants to manually optimize the carbon footprint.
* ``previous_iteration``: A previous iteration from which to perform the optimization (-1 for taking the last one).
* ``optimize_carbon_footprint_auto``: A boolean indicating if the user wants to automatically optimize the carbon footprint until a desired value.
* ``desired_carbon_footprint``: The desired carbon footprint.
* ``geo_location_continent``: The geo-location continent in which the ML problem is going to take place.
* ``geo_location_region``: The geo-location region in which the ML problem is going to take place.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #UserInput.idl

    struct GeoLocation
    {
        string continent;
        string region;
    };

    struct UserInput
    {
        string modality;
        string problem_short_description;
        string problem_definition;
        sequence<string> inputs;
        sequence<string> outputs;
        unsigned long minimum_samples;
        unsigned long maximum_samples;
        boolean optimize_carbon_footprint_manual;
        long previous_iteration;
        boolean optimize_carbon_footprint_auto;
        double desired_carbon_footprint;
        string geo_location_continent;
        string geo_location_region;
        sequence<octet> extra_data;
        @key TaskIdImpl task_id;
    };


.. _mlmodelmetadata_type:

ML Model Metadata Type
----------------------

The ``MLModelMetadata`` data structure represents the output from the ``Task Encoder``.
It is composed by:

* ``keywords``: A sequence of strings identifying the key workd from the user input problem description.
* ``ml_model_metadata``: The machine learning model metadata.
  At the current stage of development, the metadata is generically defined as a sequence of strings.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #MLModelMetadata.idl

    struct MLModelMetadata
    {
        sequence<string> keywords;
        sequence<string> ml_model_metadata;
        sequence<octet> extra_data;
        @key long task_id;
    };


.. _apprequirements_type:

Application Requirements Type
-----------------------------

The ``AppRequirements`` data structure depicts the output from the ``Application Requirements Node`` and consists on:

* ``app_requirements``: A sequence of application-level requirements, modeled as a sequence of strings, to be considered in the selection of the machine learning model.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #AppRequirements.idl

    struct AppRequirements
    {
        sequence<string> app_requirements;
        sequence<octet> extra_data;
        @key long task_id;
    };

.. _hardware_constraints_type:

Hardware Constraints Type
-------------------------

The ``HWConstraints`` represents the group of constraints defined (or not) by the user when describing the problem. It is the output from the ``Hardware Constraints Node``.

* ``max_memory_footprint``: The maximum memory footprint allowed for the ML model.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #HWConstraints.idl

    struct HWConstraints
    {
        unsigned long max_memory_footprint;
        sequence<octet> extra_data;
        @key long task_id;
    };


.. _mlmodel_type:

ML Model Type
-------------

The ``MLModel`` data structure represents the output from the ``Machine Learning Model Provider``.
It is divided in the following fields:

* ``model_path``: A string containing the path to the ``ML`` model.
* ``model``: A string with the model name, in case of remote approach.
* ``raw_model``: A sequence of bytes with the raw model, in case of remote approach.
* ``model_properties_path``: A string containing the path to the properties of the model.
* ``model_properties_path``: A string containing the path to the properties of the model.
* ``model_properties``: A string with the model properties, in case of remote approach.
* ``input_batch``: A sequence of serialized numpy arrays with a dimension: Batch x Channels x Height x Width (each one representing a batch) conforming the input batch.
* ``target_latency``: The target latency or fps for computer vision tasks or target processing latency in seconds for other tasks, like time-series analysis.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

The ``model`` and ``model_properties`` can be optionally filled.
The reasoning for include them is to overcome situations in which the model is generated into a remote machine.

.. code-block:: bash

    # MLModel.idl

    struct MLModel
    {
        string model_path;
        string model;
        sequence<octet> raw_model;
        string model_properties_path;
        string model_properties;
        sequence<string> input_batch;
        double target_latency;
        sequence<octet> extra_data;
        @key long task_id;
    };

.. _hardware_resource_type:

Hardware Resource Type
----------------------

The ``Hardware Resources Provider`` selects a best-suited energy-optimized hardware according to the ML model.
To represent that information, the ``HWResource`` data structure is defined containing the following fields:

* ``hw_description``: A string with the detailed hardware description.
* ``power_consumption``: The power consumption in ``W``.
* ``latency``: The estimation of latency of the given ONNX model for the given input batch.
* ``memory_footprint_of_ml_model``: The maximum memory footprint that can be implemented on the target FPGA.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #HWResource.idl

    struct HWResource
    {
        string hw_description;
        double power_consumption;
        double latency;
        double memory_footprint_of_ml_model;
        double max_hw_memory_footprint;
        sequence<octet> extra_data;
        @key long task_id;
    };


.. _carbonfootprint_type:

Carbon Footprint Type
---------------------

Finally, in order to model the output from the ``CO2 Footprint Provider``, the ``CO2Footprint`` data structure consisting in the following fields:

* ``carbon_footprint``: The CO2 footprint  in ``kgCO2e``.
* ``energy_consumption``: The energy consumption in ``Wh``.
* ``carbon_intensity``: The carbon intensity.
* ``extra_data``: A sequence of raw extra data for out-of-scope use cases.
* ``task_id``: The identifier of the ML problem to solve.

.. code-block:: bash

    #CO2Footprint.idl

    struct CO2Footprint
    {
        double carbon_footprint;
        double energy_consumption;
        double carbon_intensity;
        sequence<octet> extra_data;
        @key long task_id;
    };
