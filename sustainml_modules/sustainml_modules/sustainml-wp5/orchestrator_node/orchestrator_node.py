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

from . import utils

from sustainml_swig import OrchestratorNodeHandle as cpp_OrchestratorNodeHandle
from sustainml_swig import OrchestratorNode as cpp_OrchestratorNode
from sustainml_swig import NodeStatus
import sustainml_swig

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self):

        self.node_status_ = {}
        self.last_task_id = None
        # Parent class constructor
        super().__init__()

    # Callback
    def on_node_status_change(
            self,
            id: int,
            status : NodeStatus):

        if id not in self.node_status_:
            self.node_status_[id] = utils.node_status.INACTIVE.value

        self.node_status_[id] = status.node_status()
        print(utils.string_node(id), "node status", utils.string_status(status.node_status()), "received.")

    # Callback
    def on_new_node_output(
            self,
            id : int,
            data):
        task = sustainml_swig.get_task_id(id, data)
        if (self.last_task_id is None and task is not None) or (
            self.last_task_id is not None and task is not None and task > self.last_task_id):
            self.last_task_id = task
        if task is None:
            print(utils.string_node(id), "node output received.")
        else:
            print(utils.string_node(id), "node output received from task", utils.string_task(task))

class Orchestrator:

    def __init__(self):

        self.handler_ = OrchestratorNodeHandle()
        self.node_ = cpp_OrchestratorNode(self.handler_)

    # Proxy method to run the node
    def run(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate(self):

        self.node_.terminate()

    def get_last_task_id(self):
        return self.handler_.last_task_id

    def get_all_status(self):
        output = ""
        for key, value in self.handler_.node_status_.items():
            output += utils.string_node(key) + " node status " + utils.string_status(value) + "\n"
        if output == "":
            output = "No nodes have reported their status yet.\n"
        return output

    def get_status(self, node_id):
        if node_id in self.handler_.node_status_:
            return utils.string_status(self.handler_.node_status_[node_id])
        else:
            return utils.string_status(utils.node_status.INACTIVE.value)

    def get_app_requirements(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_app_requirements(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.APP_REQUIREMENTS.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        app_requirements_str_list = node_data.app_requirements()
        json_output = {'app_requirements': f'{utils.string_std_vector(app_requirements_str_list)}'}
        return json_output

    def get_model_metadata(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_model_metadata(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_METADATA.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        keywords_str_list = node_data.keywords()
        metadata_str_list = node_data.ml_model_metadata()
        json_output = {'keywords': f'{utils.string_std_vector(keywords_str_list)}',
                       'metadata': f'{utils.string_std_vector(metadata_str_list)}'}
        return json_output

    def get_hw_constraints(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_hw_constraints(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_CONSTRAINTS.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        max_value = node_data.max_memory_footprint()
        json_output = {'max_memory_footprint': f'{max_value}'}
        return json_output

    def get_ml_model_provider(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_model_provider(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_PROVIDER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        model = node_data.model()
        model_path = node_data.model_path()
        model_properties = node_data.model_properties()
        model_properties_path = node_data.model_properties_path()
        input_batch = node_data.input_batch()
        target_latency = node_data.target_latency()
        json_output = {'model': f'{model}',
                       'model_path': f'{model_path}',
                       'model_properties': f'{model_properties}',
                       'model_properties_path': f'{model_properties_path}',
                       'input_batch': f'{utils.string_std_vector(input_batch)}',
                       'target_latency': f'{target_latency}'}
        return json_output

    def get_hw_provider(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_hw_provider(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_PROVIDER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        hw_description = node_data.hw_description()
        power_consumption = node_data.power_consumption()
        latency = node_data.latency()
        memory_footprint_of_ml_model = node_data.memory_footprint_of_ml_model()
        json_output = {'hw_description': f'{hw_description}',
                       'power_consumption': f'{power_consumption}',
                       'latency': f'{latency}',
                       'memory_footprint_of_ml_model': f'{memory_footprint_of_ml_model}'}
        return json_output

    def get_carbontracker(self, task_id):
        # retrieve node data
        node_data = sustainml_swig.get_carbontracker(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.CARBONTRACKER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        carbon_footprint = node_data.carbon_footprint()
        energy_consumption = node_data.energy_consumption()
        carbon_intensity = node_data.carbon_intensity()
        json_output = {'carbon_footprint': f'{carbon_footprint}',
                       'energy_consumption': f'{energy_consumption}',
                       'carbon_intensity': f'{carbon_intensity}'}
        return json_output

    def get_results(self, node_id, task_id):
        if node_id == utils.node_id.APP_REQUIREMENTS.value:
            return self.get_app_requirements(task_id)
        elif node_id == utils.node_id.ML_MODEL_METADATA.value:
            return self.get_model_metadata(task_id)
        elif node_id == utils.node_id.HW_CONSTRAINTS.value:
            return self.get_hw_constraints(task_id)
        elif node_id == utils.node_id.ML_MODEL_PROVIDER.value:
            return self.get_ml_model_provider(task_id)
        elif node_id == utils.node_id.HW_PROVIDER.value:
            return self.get_hw_provider(task_id)
        elif node_id == utils.node_id.CARBONTRACKER.value:
            return self.get_carbontracker(task_id)
        else:
            return utils.string_node(node_id) + " node does not have any results to show."
