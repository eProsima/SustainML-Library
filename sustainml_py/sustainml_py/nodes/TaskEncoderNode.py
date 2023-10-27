# Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

from sustainml_swig import TaskEncoderTaskListener as cpp_TaskEncoderTaskListener
from sustainml_swig import TaskEncoderNode as cpp_TaskEncoderNode

from sustainml_swig import EncodedTask, NodeStatus, UserInput

class TaskEncoderTaskListener(cpp_TaskEncoderTaskListener):

    def __init__(self,
                 callback):

        self.callback_ = callback

        # Parent class constructor
        super().__init__()

    # Callback
    def on_new_task_available(
            self,
            user_input : UserInput,
            node_status : NodeStatus,
            encoded_task : EncodedTask):

        """ Invoke user callback """
        self.callback_(user_input, node_status, encoded_task)

# Proxy class to instantiate by the user
class TaskEncoderNode:

    def __init__(self,
                 callback = None):

        if callback == None:
            raise ValueError(
                'TaskEncoderNode constructor expects a callback.')

        self.listener_ = TaskEncoderTaskListener(callback)
        self.node_ = cpp_TaskEncoderNode(self.listener_)

    # Proxy method to run the node
    def spin(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate():

        cpp_TaskEncoderNode.terminate()