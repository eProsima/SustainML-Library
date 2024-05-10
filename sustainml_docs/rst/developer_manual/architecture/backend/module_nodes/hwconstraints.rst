.. _hwconstraints_node:

Hardware-Constraints Node
=========================

.. raw:: html

   <style>
        .module_node:not(#hardware-constraints) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#hardware-constraints) {
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


The :ref:`hwconstraints_node` is responsible for imposing hardware constraints for the generation of the machine learning model proposal.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`hwconstraints_node`:

+----------------------+-------------------+--------------------------------+---------------------------+
| Input                | From Node         | Output                         | To Node                   |
+----------------------+-------------------+--------------------------------+---------------------------+
|:ref:`user_input_type`|:ref:`orchestrator`|:ref:`hardware_constraints_type`|:ref:`mlmodelprovider_node`|
+----------------------+-------------------+--------------------------------+---------------------------+
|                      |                   |:ref:`hardware_constraints_type`|:ref:`hwprovider_node`     |
+----------------------+-------------------+--------------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`hwconstraints_node`.
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
    """SustainML HW Constraints Node Implementation."""

    from sustainml_py.nodes.HardwareConstraintsNode import HardwareConstraintsNode

    # Manage signaling
    import signal
    import threading
    import time

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        HardwareConstraintsNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: user_input
    # Outputs: node_status, hw_constraints
    def task_callback(user_input, node_status, hw_constraints):

        # Callback implementation here

        # Read the inputs
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
        task_id = user_input.task_id()

        # Do processing...

        # Populate hardware constraints output.
        # There is no need to specify node_status for the moment
        # as it will automatically be set to IDLE when the callback returns.

        hw_constraints.max_memory_footprint(100)

    # Main workflow routine
    def run():
        node = HardwareConstraintsNode(callback=task_callback)
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
