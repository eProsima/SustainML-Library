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

.. code-block:: python

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
    """SustainML ML Model Provider Node Implementation."""

    from sustainml_py.nodes.MLModelNode import MLModelNode

    # Manage signaling
    import signal
    import threading
    import time

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

        # Callback implementation here

        # Read the inputs
        # MLModelMetadata
        for metadata_prop in ml_model_metadata.ml_model_metadata():
            print(metadata_prop)
        for keyword in ml_model_metadata.keywords():
            print(keyword)

        # Apprequirements
        for requirement in app_requirements.app_requirements():
            print(requirement)

        # HWConstraints
        max_mem_footprint = hw_constraints.max_memory_footprint()
        hw_required = hw_constraints.hardware_required()

        # MLModel
        # Only in case of optimization (task_id.iteration_id() > 1)
        model_path = ml_model_baseline.model_path()
        model = ml_model_baseline.model()
        model_properties_path = ml_model_baseline.model_properties_path()
        model_properties = ml_model_baseline.model_properties()
        for input_batch in ml_model_baseline.input_batch():
            print(input_batch)
        target_latency = ml_model_baseline.target_latency()

        # HWResource
        # Only in case of optimization (task_id.iteration_id() > 1)
        hw_description = hw_baseline.hw_description()
        power_consumption = hw_baseline.power_consumption()
        latency = hw_baseline.latency()
        memory_footprint_of_ml_model = hw_baseline.memory_footprint_of_ml_model()
        max_hw_memory_footprint = hw_baseline.max_hw_memory_footprint()

        # CarbonFootprint
        # Only in case of optimization (task_id.iteration_id() > 1)
        carbon_footprint = carbonfootprint_baseline.carbon_footprint()
        energy_consumption = carbonfootprint_baseline.energy_consumption()
        carbon_intesity = carbonfootprint_baseline.carbon_intesity()

        # Do processing...

        # Populate ml model porvider output.
        # There is no need to specify node_status for the moment
        # as it will automatically be set to IDLE when the callback returns.
        ml_model.model_path("")
        ml_model.model("")
        ml_model.model_properties_path("")
        ml_model.model_properties("")
        ml_model.input_batch().append("batch"):
        ml_model.target_latency(800)

    # Main workflow routine
    def run():
        node = MLModelNode(callback=task_callback)
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
