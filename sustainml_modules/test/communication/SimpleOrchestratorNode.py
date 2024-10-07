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

import threading

from sustainml_swig import OrchestratorNodeHandle as cpp_OrchestratorNodeHandle
from sustainml_swig import OrchestratorNode as cpp_OrchestratorNode

from sustainml_swig import NodeStatus

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self):
        self.node_data_received_ = {}
        # Condition variable
        self.cv_ = threading.Condition()

        # Parent class constructor
        super().__init__()

    # Callback
    def on_node_status_change(
            self,
            id: int,
            status : NodeStatus):

        if id not in self.node_data_received_:
            self.node_data_received_[id] = (0, 0)

        self.node_data_received_[id] = (status.node_status(), self.node_data_received_[id][1])

        self.cv_.acquire()
        self.cv_.notify()
        self.cv_.release()

    # Callback
    def on_new_node_output(
            self,
            id : int,
            data):

        if id not in self.node_data_received_:
            self.node_data_received_[id] = (0, 0)

        self.node_data_received_[id] = (self.node_data_received_[id][0], 1 + self.node_data_received_[id][1])

        self.cv_.acquire()
        self.cv_.notify()
        self.cv_.release()

        print("NodeOutput received ", id)

    def prepare_expected_data(
            self,
            expected_data):

        self.expected_data_ = expected_data

    def check_data(
            self):
        for key, value in self.node_data_received_.items():
            print(f"Arrived {key} {value[0]} {value[1]}")

        for key, value in self.expected_data_.items():
            print(f"Expected {key} {value[0]} {value[1]}")

        return self.expected_data_ == self.node_data_received_

    def wait_for_data(
            self):
        print("Waiting for data")
        with self.cv_:
            while not self.check_data():
                print("Waiting for data...")
                self.cv_.wait()
            print("Data received")

# Proxy class to instantiate by the user
class OrchestratorNode:

    def __init__(self):

        self.handler_ = OrchestratorNodeHandle()
        self.node_ = cpp_OrchestratorNode(self.handler_)

    # Proxy method to run the node
    def spin(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate():

        cpp_OrchestratorNode.terminate()

# Call main in program execution
if __name__ == '__main__':

    expected_data_nodes_ready = {
        # Status::NODE_IDLE (2), count (0)
        0: (2, 0), 1: (2, 0), 2: (2, 0), 3: (2, 0), 4: (2, 0), 5: (2, 0)
    }

    expected_data_task_executed = {
        # Status::NODE_IDLE (2), count (1)
        0: (2, 1), 1: (2, 1), 2: (2, 1), 3: (2, 1), 4: (2, 1), 5: (2, 1)
    }
    node = OrchestratorNode()
    node.handler_.prepare_expected_data(expected_data_nodes_ready)
    node.handler_.wait_for_data()
    node.handler_.prepare_expected_data(expected_data_task_executed)
    new_task = node.node_.prepare_new_task()
    task_id = new_task[0]
    ui = new_task[1]
    ui.task_id(task_id)
    ui.modality("video")
    ui.problem_definition("Classify cars in a video sequence.")
    node.node_.start_task(task_id, ui)
    node.handler_.wait_for_data()
