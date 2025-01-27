# Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

from sustainml_swig import NodeStatus, RequestType, ResponseType

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
    def terminate(self):

        self.node_.terminate()
        self.node_.destroy()

    def send_request(self, json_data):
        request_type = RequestType()
        request_type.node_id(json_data.get('node_id'))
        request_type.transaction_id(json_data.get('transaction_id'))
        request_type.configuration(json_data.get('configuration'))

        if request_type.node_id() is None or request_type.transaction_id() is None or request_type.configuration() is None:
            return None
        res = self.node_.configuration_request(request_type)
        if res.success:
            return res
        else:
            return None

# Call main in program execution
if __name__ == '__main__':

    node = OrchestratorNode()
    print("Node created")
    res = node.send_request({
        "node_id": 0,
        "transaction_id": 1,
        "configuration": "dummy1"
    })
    print(f"Response: {res.success()}")
    res = node.send_request({
        "node_id": 1,
        "transaction_id": 1,
        "configuration": "dummy2"
    })
    res = node.send_request({
        "node_id": 2,
        "transaction_id": 1,
        "configuration": "dummy3"
    })
    res = node.send_request({
        "node_id": 3,
        "transaction_id": 1,
        "configuration": "dummy4"
    })
    res = node.send_request({
        "node_id": 4,
        "transaction_id": 1,
        "configuration": "dummy5"
    })
    res = node.send_request({
        "node_id": 5,
        "transaction_id": 1,
        "configuration": "dummy6"
    })
    node.terminate()
