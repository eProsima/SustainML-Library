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

from sustainml_swig import MLModelTaskListener as cpp_MLModelTaskListener
from sustainml_swig import MLModelNode as cpp_MLModelNode
from sustainml_swig import RequestReplyListener as cpp_RequestReplyListener


from sustainml_swig import MLModel, NodeStatus, MLModelMetadata, HWConstraints, AppRequirements, HWResource, CO2Footprint, RequestType, ResponseType

class MLModelTaskListener(cpp_MLModelTaskListener):

    def __init__(self,
                 callback):

        self.callback_ = callback

        # Parent class constructor
        super().__init__()

    # Callback
    def on_new_task_available(
            self,
            ml_model_metadata : MLModelMetadata,
            app_requirements : AppRequirements,
            hw_constraints : HWConstraints,
            ml_model_baseline : MLModel,
            hw_baseline : HWResource,
            carbonfootprint_baseline : CO2Footprint,
            node_status : NodeStatus,
            ml_model : MLModel):

        """ Invoke user callback """
        self.callback_(ml_model_metadata, app_requirements, hw_constraints, ml_model_baseline, hw_baseline, carbonfootprint_baseline, node_status, ml_model)

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
class MLModelNode:

    def __init__(self,
                 callback = None,
                 service_callback = None):

        if callback == None:
            raise ValueError(
                'MLModelNode constructor expects a callback.')

        if service_callback == None:
            raise ValueError(
                'MLModelNode constructor expects a service callback.')

        self.listener_ = MLModelTaskListener(callback)
        self.listener_service_ = RequestReplyListener(service_callback)

        self.node_ = cpp_MLModelNode(self.listener_, self.listener_service_)

    # Proxy method to run the node
    def spin(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate():

        cpp_MLModelNode.terminate()
