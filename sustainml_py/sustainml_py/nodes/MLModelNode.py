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

import asyncio
import logging
from sustainml_swig import MLModelTaskListener as cpp_MLModelTaskListener
from sustainml_swig import MLModelNode as cpp_MLModelNode
from sustainml_swig import RequestReplyListener as cpp_RequestReplyListener


from sustainml_swig import MLModel, NodeStatus, MLModelMetadata, HWConstraints, AppRequirements, HWResource, CO2Footprint, RequestType, ResponseType

from ..AsyncTaskManager import get_task_manager

class MLModelTaskListener(cpp_MLModelTaskListener):

    def __init__(self,
                 callback):

        self.callback_ = callback
        self.task_manager = get_task_manager()

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
        task_id = ml_model.task_id()

        async def async_callback():
            if asyncio.iscoroutinefunction(self.callback_):
                await self.callback_(ml_model_metadata, app_requirements, hw_constraints,
                                   ml_model_baseline, hw_baseline, carbonfootprint_baseline,
                                   node_status, ml_model)
            else:
                self.callback_(ml_model_metadata, app_requirements, hw_constraints,
                              ml_model_baseline, hw_baseline, carbonfootprint_baseline,
                              node_status, ml_model)

        # Start the task with cancellation support
        success = self.task_manager.start_task(task_id, async_callback)
        if not success:
            logging.warning(f"Failed to start async task for {task_id.problem_id()}.{task_id.iteration_id()}")

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
        self.task_manager = get_task_manager()

        self.node_ = cpp_MLModelNode(self.listener_, self.listener_service_)

        # Register the stop callback with the C++ node
        self._register_stop_callback()

    def _register_stop_callback(self):
        """Register the stop callback with the underlying C++ implementation."""
        def stop_callback(task_id):
            logging.info(f"Stopping MLModel task {task_id.problem_id()}.{task_id.iteration_id()}")
            self.task_manager.stop_task(task_id)

        # Register the callback with the C++ node if the method is available
        if hasattr(self.node_, 'register_stop_task_callback'):
            self.node_.register_stop_task_callback(stop_callback)
        else:
            logging.warning("C++ node does not support stop task callback registration")

    # Proxy method to run the node
    def spin(self):
        self.node_.spin()

    def stop_task(self, task_id):
        """Public method to stop a specific task."""
        return self.task_manager.stop_task(task_id)

    def stop_all_tasks(self):
        """Stop all running tasks."""
        self.task_manager.stop_all_tasks()

    # Proxy method to manually terminate
    @staticmethod
    def terminate():
        get_task_manager().shutdown()
        cpp_MLModelNode.terminate()
