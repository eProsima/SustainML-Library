.. _appreqs_node:

Application-Requirements Node
=============================

.. raw:: html

   <style>
        .module_node:not(#application-requirements) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#application-requirements) {
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

The :ref:`appreqs_node` is meant to turn the information given by the user into application requirements, the rules that the rest of the nodes must follow when building and deploying the machine learning model.

In the current version of the framework this node is a placeholder: it does not analyse the user input yet.
For every task it publishes the same fixed list of requirements, ``["Im", "A", "New", "Requirement"]``, and the requirements are not used by any other node.
It must be running anyway, because the :ref:`mlmodelprovider_node` and the :ref:`hwprovider_node` only start a task once they have received all their inputs, the :ref:`apprequirements_type` of this node included.

The node implementation is the ``app_requirements_node.py`` file of the WP1 module (``sustainml_modules/sustainml_modules/sustainml-wp1``).

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`appreqs_node`:

+----------------------+-------------------+---------------------------+---------------------------+
| Input                | From Node         | Output                    | To Node                   |
+----------------------+-------------------+---------------------------+---------------------------+
|:ref:`user_input_type`|:ref:`orchestrator`|:ref:`apprequirements_type`|:ref:`mlmodelprovider_node`|
+----------------------+-------------------+---------------------------+---------------------------+
|                      |                   |:ref:`apprequirements_type`|:ref:`hwprovider_node`     |
+----------------------+-------------------+---------------------------+---------------------------+
|                      |                   |:ref:`apprequirements_type`|:ref:`orchestrator`        |
+----------------------+-------------------+---------------------------+---------------------------+

The node fills only the ``app_requirements`` list of the output; its ``extra_data`` is left empty.
The :ref:`orchestrator` stores the output so that the :ref:`frontend` can show it with the rest of the task results.

Configuration Requests
----------------------

The node answers every :ref:`configuration request <node_config_service>` with the same dummy JSON and ``success`` set to ``true``:

.. code-block:: json

    {"param1": "value1", "param2": "value2", "param3": "value3"}

No configuration request of the :ref:`frontend` is meant for this node.
However, the orchestrator of the back-end routes each request by the keywords found in its ``configuration`` string
(``modality`` and ``dataset_path`` to the :ref:`mlmodelmetadata_node`, ``model_from_goal`` and ``hf_search`` to the :ref:`mlmodelprovider_node`, and so on),
and a request that matches no keyword keeps the default ``node_id`` ``0``, which is the ID of this node.
So receiving the dummy reply above means that the request did not match any node.

Node Template
-------------

The following is the Python API of the :ref:`appreqs_node`, which is also the complete current implementation of the node, ``sustainml-wp1/app_requirements_node.py`` (`full file on GitHub <https://github.com/eProsima/sustainml-wp1/blob/main/app_requirements_node.py>`__).
The user implements the functionality of the node in ``task_callback()``, filling the ``app_requirements`` output from the ``user_input``,
and the response to the configuration request from the orchestrator in ``configuration_callback()``.

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
    """SustainML App Requirements Node Implementation."""

    from sustainml_py.nodes.AppRequirementsNode import AppRequirementsNode

    # Manage signaling
    import signal
    import threading
    import time
    import json

    # Whether to go on spinning or interrupt
    running = False

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        AppRequirementsNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: user_input
    # Outputs: node_status, app_requirements
    def task_callback(user_input, node_status, app_requirements):

        # Callback implementation here

        app_requirements.app_requirements().append("Im")
        app_requirements.app_requirements().append("A")
        app_requirements.app_requirements().append("New")
        app_requirements.app_requirements().append("Requirement")

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here

        # Dummy JSON configuration and implementation
        dummy_config = {
            "param1": "value1",
            "param2": "value2",
            "param3": "value3"
        }
        res.configuration(json.dumps(dummy_config))
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())
        res.success(True)
        res.err_code(0) # 0: No error || 1: Error

    # Main workflow routine
    def run():
        node = AppRequirementsNode(callback=task_callback, service_callback=configuration_callback)
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
