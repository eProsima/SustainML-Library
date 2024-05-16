.. _hwprovider_node:

Hardware-Resource-Provider Node
===============================

.. raw:: html

   <style>
        .module_node:not(#hardware-resource-provider) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#hardware-resource-provider) {
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

The :ref:`hwprovider_node` selects the hardware resource that optimally fits the proposed machine learning model.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`hwprovider_node`:

+--------------------------------+---------------------------+-----------------------------+---------------------------+
| Input                          | From Node                 | Output                      | To Node                   |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`apprequirements_type`     |:ref:`appreqs_node`        |:ref:`hardware_resource_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`hardware_resource_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`hardware_constraints_type`|:ref:`hwconstraints_node`  |:ref:`hardware_resource_type`|:ref:`carbontracker_node`  |
+--------------------------------+---------------------------+-----------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`hwprovider_node`.
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
    """SustainML HW Resources Provider Node Implementation."""

    from sustainml_py.nodes.HardwareResourcesNode import HardwareResourcesNode

    # Manage signaling
    import signal
    import threading
    import time

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        HardwareResourcesNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: ml_model, app_requirements, hw_constraints
    # Outputs: node_status, hw
    def task_callback(ml_model, app_requirements,  hw_constraints, node_status, hw):

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
        task_id = ml_model.task_id()

        # Apprequirements
        for requirement in app_requirements.app_requirements():
            print(requirement)

        # HWConstraints
        hw_constraint = hw_constraints.max_memory_footprint()

        # Do processing...

        # Populate hardware resource output.
        # There is no need to specify node_status for the moment
        # as it will automatically be set to IDLE when the callback returns.
        hw.hw_description("Description of the hardware goes here")
        hw.power_consumption(44)
        hw.latency(334)
        hw.memory_footprint_of_ml_model(23)
        hw.max_hw_memory_footprint(23)

    # Main workflow routine
    def run():
        node = HardwareResourcesNode(callback=task_callback)
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
