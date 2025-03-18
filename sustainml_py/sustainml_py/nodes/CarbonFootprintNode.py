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

# Import node
from sustainml_swig import CarbonFootprintTaskListener as cpp_CarbonFootprintTaskListener
from sustainml_swig import CarbonFootprintNode as cpp_CarbonFootprintNode
from sustainml_swig import RequestReplyListener as cpp_RequestReplyListener

# Import types
from sustainml_swig import MLModel, UserInput, HWResource, NodeStatus, CO2Footprint, RequestType, ResponseType

# Class implementing node listener
class CarbonFootprintTaskListener(cpp_CarbonFootprintTaskListener):

    def __init__(self,
                 callback):

        self.callback_ = callback

        # Parent class constructor
        super().__init__()

    # Callback
    def on_new_task_available(
            self,
            ml_model : MLModel,
            user_input : UserInput,
            hw : HWResource,
            node_status : NodeStatus,
            co2 : CO2Footprint):

        """ Invoke user callback """
        self.callback_(ml_model, user_input, hw, node_status, co2)

class RequestReplyListener(cpp_RequestReplyListener):

    def __init__(self,
                 callback):

        self.callback_ = callback

        # Parent class constructor
        super().__init__()

    # Callback
    def on_configuration_request(
            self,
            req : RequestType,
            res : ResponseType):

        """ Invoke user callback """
        self.callback_(req, res)

# Proxy class to instantiate by the user
class CarbonFootprintNode:

    def __init__(self,
                 callback = None,
                 service_callback = None):

        if callback == None:
            raise ValueError(
                'CarbonFootprintNode constructor expects a callback.')

        if service_callback == None:
            raise ValueError(
                'CarbonFootprintNode constructor expects a service callback.')

        self.listener_ = CarbonFootprintTaskListener(callback)
        self.listener_service_ = RequestReplyListener(service_callback)
        self.node_ = cpp_CarbonFootprintNode(self.listener_, self.listener_service_)

    # Proxy method to run the node
    def spin(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate():

        cpp_CarbonFootprintNode.terminate()
