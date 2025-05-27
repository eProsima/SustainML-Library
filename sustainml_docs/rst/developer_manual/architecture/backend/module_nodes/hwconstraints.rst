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
And inside ``configuration_callback()`` implement the response to the configuration request from the orchestrator.

.. code-block:: python

    # Copyright 2023 SustainML Consortium
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

    from sustainml_py.nodes.HardwareConstraintsNode import HardwareConstraintsNode

    # Manage signaling
    import ctypes
    import json
    import numpy as np
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

        # Default values
        hw_req = "PIM_AI_1chip"
        mem_footprint = 100

        # Check if extra data has been sent
        if user_input.extra_data().size() != 0:
            buffer = ctypes.c_ubyte * user_input.extra_data().size()
            buffer = buffer.from_address(int(user_input.extra_data().get_buffer()))
            extra_data = np.frombuffer(buffer, dtype=np.uint8)
            extra_data_str = extra_data.tobytes().decode('utf-8', errors='ignore')
            try:
                json_obj = json.loads(extra_data_str)
                if json_obj is not None:
                    mem_footprint = int(json_obj["max_memory_footprint"])
                    hw_req = json_obj["hardware_required"]
                    hf_token = json_obj["hf_token"]
                    encoded_data = json.dumps({"hf_token": hf_token}).encode("utf-8")
                    hw_constraints.extra_data(encoded_data)
            except:
                print("Extra data is not a valid JSON object, using default values")

        # TODO parse other possible data hidden in the extra_data field, if any
        # TODO populate the hw_constraints object with the required data

        hw_constraints.max_memory_footprint(mem_footprint)
        hw_constraints.hardware_required([hw_req])

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here

        # Case not supported
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())
        error_msg = f"Unsupported configuration request: {req.configuration()}"
        res.configuration(json.dumps({"error": error_msg}))
        res.success(False)
        res.err_code(1) # 0: No error || 1: Error
        print(error_msg)


    # Main workflow routine
    def run():
        node = HardwareConstraintsNode(callback=task_callback, service_callback=configuration_callback)
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
