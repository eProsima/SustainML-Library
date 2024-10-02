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
"""SustainML Client Node API specification."""

from sustainml_swig import OrchestratorNodeHandle as cpp_OrchestratorNodeHandle
from sustainml_swig import OrchestratorNode as cpp_OrchestratorNode

from sustainml_swig import NodeStatus

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self,
                 new_node_output_callback,
                 node_status_change_callback):

        self.new_node_output = new_node_output_callback
        self.node_status_change = node_status_change_callback

        # Parent class constructor
        super().__init__()

    # Callback
    def on_new_node_output(
            self,
            id : int,
            data):

        """ Invoke user callback """
        self.new_node_output(id, data)

    # Callback
    def on_node_status_change(
            self,
            id: int,
            status : NodeStatus):

        """ Invoke user callback """
        self.node_status_change(id, status)

# Proxy class to instantiate by the user
class OrchestratorNode:

    def __init__(self,
                 new_node_output = None,
                 node_status_change = None):

        if new_node_output == None:
            raise ValueError(
                'OrchestratorNode constructor expects a new_node_output callback.')

        if node_status_change == None:
            raise ValueError(
                'OrchestratorNode constructor expects a node_status_change callback.')

        self.handler_ = OrchestratorNodeHandle(
                new_node_output,
                node_status_change)
        self.node_ = cpp_OrchestratorNode(self.handler_)

    # Proxy method to run the node
    def spin(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate():

        cpp_OrchestratorNode.terminate()
