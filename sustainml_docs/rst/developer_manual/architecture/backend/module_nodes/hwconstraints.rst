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

The :ref:`hwconstraints_node` turns the hardware constraints chosen by the user when defining the problem into a :ref:`hardware_constraints_type` sample:
the hardware on which the machine learning model must run and the maximum memory footprint allowed for it.
It is implemented in ``hw_constraints_node.py`` of the ``sustainml-wp2`` module.

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
|                      |                   |:ref:`hardware_constraints_type`|:ref:`orchestrator`        |
+----------------------+-------------------+--------------------------------+---------------------------+

Node Behavior
-------------

The hardware constraints are not read from the regular fields of the :ref:`user_input_type`, but from the JSON object carried in its ``extra_data`` field.
The orchestrator fills this JSON object with the data of the problem definition sent by the frontend.
The node reads the following keys:

* ``hardware_required``: name of the hardware on which the model will be evaluated.
  It is one of the names returned by the :ref:`hwprovider_node` for the ``hardwares`` configuration request.
  If it is missing, ``PIM_AI_1chip`` is used.
* ``max_memory_footprint``: maximum memory footprint of the model, in MB.
  If it is missing, ``0`` is used, which means that there is no limit.

The node then fills the :ref:`hardware_constraints_type` output as follows:

* ``hardware_required``: a sequence with the single hardware name read above.
* ``max_memory_footprint``: the maximum memory footprint read above (MB, ``0`` means no limit).
* ``extra_data``: a copy of the whole JSON object received in the ``extra_data`` of the :ref:`user_input_type`.
  This forwards to the following nodes the rest of the problem definition, such as the model family (``model_family``, ``Transformers`` or ``CNNs``) and the Hugging Face token (``hf_token``),
  which the :ref:`hwprovider_node` uses to choose how to evaluate the model and to download it.

The node does not check the hardware name: the :ref:`hwprovider_node` is the one that evaluates the model on it.

Configuration Requests
----------------------

The :ref:`hwconstraints_node` does not answer any configuration request.
Any request it receives is answered with ``success`` set to ``false``, ``err_code`` set to ``1`` and the following ``configuration`` JSON:

.. code-block:: json

    {"error": "Unsupported configuration request: <request>"}

Node Template
-------------

Following is the current implementation of the :ref:`hwconstraints_node` (``sustainml-wp2/hw_constraints_node.py``, `full file on GitHub <https://github.com/eProsima/sustainml-wp2/blob/main/hw_constraints_node.py>`__), built on the Python API of ``sustainml_py``.
The functionality of the node is implemented within ``task_callback()``, which receives the :ref:`user_input_type` and fills the :ref:`hardware_constraints_type` output.
The response to the configuration requests forwarded by the orchestrator is implemented within ``configuration_callback()``.

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
    """SustainML HW Constraints Node Implementation."""

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
        mem_footprint = 0  # MB, 0 means no limit

        # Check if extra data has been sent and preserve ALL fields (hf_token, model_family, …)
        incoming = {}
        if user_input.extra_data().size() != 0:
            try:
                buffer = ctypes.c_ubyte * user_input.extra_data().size()
                buffer = buffer.from_address(int(user_input.extra_data().get_buffer()))
                extra_data = np.frombuffer(buffer, dtype=np.uint8)
                extra_data_bytes = extra_data.tobytes()
                extra_data_str = extra_data_bytes.decode('utf-8', errors='ignore')
                incoming = json.loads(extra_data_str) if extra_data_str else {}
            except Exception as e:
                print("[HW_CONSTRAINTS] WARN: could not decode extra_data from user_input:", e)
                incoming = {}

        # Pull values (use defaults if missing)
        mem_footprint = int(incoming.get("max_memory_footprint", mem_footprint))
        hw_req = incoming.get("hardware_required", hw_req)

        # Build outgoing extra_data preserving everything we received
        out_extra = dict(incoming)  # copy

        # Forward the full extra_data downstream so HW_RESOURCES can read model_family
        try:
            hw_constraints.extra_data(json.dumps(out_extra).encode("utf-8"))
        except Exception as e:
            print("[HW_CONSTRAINTS] WARN: cannot set hw_constraints.extra_data:", e)

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
