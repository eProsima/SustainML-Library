# Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
"""SustainML Orchestrator Node API specification."""

from . import utility

from sustainml_swig import OrchestratorNodeHandle as cpp_OrchestratorNodeHandle
from sustainml_swig import OrchestratorNode as cpp_OrchestratorNode
from sustainml_swig import NodeStatus

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self):

        self.node_status_ = {}
        # Parent class constructor
        super().__init__()

    # Callback
    def on_node_status_change(
            self,
            id: int,
            status : NodeStatus):

        if id not in self.node_status_:
            self.node_status_[id] = utility.node_status.INACTIVE.value

        self.node_status_[id] = status.node_status()
        print(utility.string_node(id), "node status", utility.string_status(status.node_status()), "received.")

    # Callback
    def on_new_node_output(
            self,
            id : int,
            data):
        # data cannot be casted here
        print(utility.string_node(id), "node output received.")

class Orchestrator:

    def __init__(self):

        self.handler_ = OrchestratorNodeHandle()
        self.node_ = cpp_OrchestratorNode(self.handler_)

    # Proxy method to run the node
    def run(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate(self):

        cpp_OrchestratorNode.terminate()

    def get_all_status(self):
        output = ""
        for key, value in self.handler_.node_status_.items():
            output += utility.string_node(key) + " node status " + utility.string_status(value) + "\n"
        if output == "":
            output = "No nodes have reported their status yet.\n"
        return output
