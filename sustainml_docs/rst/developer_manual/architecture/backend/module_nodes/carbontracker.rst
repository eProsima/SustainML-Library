.. _carbontracker_node:

Carbontracker Node
==================

.. raw:: html

   <style>
        .module_node:not(#carbontracker) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#carbontracker) {
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

The :ref:`carbontracker_node` is responsible for estimating the carbon footprint of the machine learning model on the selected hardware.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`carbontracker_node`:

+--------------------------------+---------------------------+---------------------------+---------------------------+
| Input                          | From Node                 | Output                    | To Node                   |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`user_input_type`          |:ref:`orchestrator`        |:ref:`carbonfootprint_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`carbonfootprint_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_resource_type`   |:ref:`hwprovider_node`     |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`carbontracker_node`.
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
    """SustainML Carbon Tracker Node Implementation."""

    from sustainml_py.nodes.CarbonFootprintNode import CarbonFootprintNode

    # Manage signaling
    import signal
    import threading
    import time

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        CarbonFootprintNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: ml_model, user_input, hw
    # Outputs: node_status, co2
    def task_callback(ml_model, user_input, hw, node_status, co2):

        # Callback implementation here

        # Read the inputs
        # MLModel
        model_path = ml_model.model_path()
        model = ml_model.model()
        model_properties_path = ml_model.model_properties_path()
        model_properties = ml_model.model_properties()
        for input_batch in ml_model.input_batch():
            print(input_batch)
        target_latency = ml_model.target_latency()

        # UserInput
        # (some of the fields are for internal use only and will not be shown here)
        modality = user_input.modality()
        task_name = user_input.problem_short_description()
        problem_definition = user_input.problem_definition()
        for input in user_input.inputs():
            print(input)
        for output in user_input.outputs():
            print(output)
        min_samples = user_input.minimum_samples()
        max_samples = user_input.maximum_samples()
        continent = user_input.continent()
        region = user_input.region()
        for byte in user_input.extra_data():
            print(byte)

        # HWResource
        hw_description = hw.hw_description()
        power_consumption = hw.power_consumption()
        latency = hw.latency()
        memory_footprint_of_ml_model = hw.memory_footprint_of_ml_model()
        max_hw_memory_footprint = hw.max_hw_memory_footprint()

        # Do processing...

        # Populate carbon footprint output.
        # There is no need to specify node_status for the moment
        # as it will automatically be set to IDLE when the callback returns.
        co2.carbon_footprint(4)
        co2.energy_consumption(1000)
        co2.carbon_intesity(2)


    # Main workflow routine
    def run():
        global running
        running = True
        node = CarbonFootprintNode(callback=task_callback)
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
