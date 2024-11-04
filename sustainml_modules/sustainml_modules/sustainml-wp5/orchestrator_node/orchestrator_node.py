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
import numpy as np

from sustainml_swig import OrchestratorNodeHandle as cpp_OrchestratorNodeHandle
from sustainml_swig import OrchestratorNode as cpp_OrchestratorNode
from sustainml_swig import NodeStatus
import sustainml_swig
import threading

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self):

        self.condition = threading.Condition()
        self.last_task_id = None
        self.node_status_ = {}
        self.result_status = {}
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
        task_id = sustainml_swig.get_task_id(id, data)
        if task_id is None:
            print(utils.string_node(id), "node output received.")
        else:
            print(utils.string_node(id), "node output received from task", utils.string_task(task_id))
        self.register_result(task_id, id)

    def register_task(self, task_id):
        with self.condition:
            if (self.last_task_id is None and task_id is not None) or (
                self.last_task_id is not None and task_id is not None and task_id > self.last_task_id):
                self.last_task_id = task_id
            self.result_status[utils.string_task(task_id)] = {
                utils.node_id.APP_REQUIREMENTS.value: False,
                utils.node_id.CARBONTRACKER.value: False,
                utils.node_id.HW_CONSTRAINTS.value: False,
                utils.node_id.HW_PROVIDER.value: False,
                utils.node_id.ML_MODEL_METADATA.value: False,
                utils.node_id.ML_MODEL_PROVIDER.value: False
            }

    def register_result(self, task_id, node_id):
        with self.condition:
            if utils.string_task(task_id) not in self.result_status:
                self.register_task(task_id)
            self.result_status[utils.string_task(task_id)][node_id] = True
            self.condition.notify_all()

    def results_available(self, task_id, node_id):
        with self.condition:
            return self.result_status[utils.string_task(task_id)].get(node_id, False)

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
            output += utils.string_node(key) + " node status " + utils.string_status(value) + "<br>"
        if output == "":
            output = "No nodes have reported their status yet.\n"
        return output

    def get_status(self, node_id):
        if node_id in self.handler_.node_status_:
            return utils.string_status(self.handler_.node_status_[node_id])
        else:
            return utils.string_status(utils.node_status.INACTIVE.value)

    def get_app_requirements(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.APP_REQUIREMENTS.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_app_requirements(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.APP_REQUIREMENTS.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        app_requirements_str_list = node_data.app_requirements()
        json_output = {'app_requirements': f'{utils.string_std_vector(app_requirements_str_list)}<br>'}
        return json_output

    def get_model_metadata(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.ML_MODEL_METADATA.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_model_metadata(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_METADATA.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        keywords_str_list = node_data.keywords()
        metadata_str_list = node_data.ml_model_metadata()
        json_output = {'keywords': f'{utils.string_std_vector(keywords_str_list)}<br>',
                       'metadata': f'{utils.string_std_vector(metadata_str_list)}<br>'}
        return json_output

    def get_hw_constraints(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.HW_CONSTRAINTS.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_hw_constraints(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_CONSTRAINTS.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        max_value = node_data.max_memory_footprint()
        required_hardware = node_data.hardware_required()
        json_output = {'max_memory_footprint': f'{max_value}<br>',
                       'hardware_required': f'{utils.string_std_vector(required_hardware)}<br>'}
        return json_output

    def get_ml_model_provider(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.ML_MODEL_PROVIDER.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_model_provider(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_PROVIDER.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        model = node_data.model()
        model_path = node_data.model_path()
        model_properties = node_data.model_properties()
        model_properties_path = node_data.model_properties_path()
        input_batch = node_data.input_batch()
        target_latency = node_data.target_latency()
        json_output = {'model': f'{model}<br>',
                       'model_path': f'{model_path}<br>',
                       'model_properties': f'{model_properties}<br>',
                       'model_properties_path': f'{model_properties_path}<br>',
                       'input_batch': f'{utils.string_std_vector(input_batch)}<br>',
                       'target_latency': f'{target_latency}<br>'}
        return json_output

    def get_hw_provider(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.HW_PROVIDER.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_hw_provider(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_PROVIDER.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        hw_description = node_data.hw_description()
        power_consumption = node_data.power_consumption()
        latency = node_data.latency()
        memory_footprint_of_ml_model = node_data.memory_footprint_of_ml_model()
        json_output = {'hw_description': f'{hw_description}<br>',
                       'power_consumption': f'{power_consumption}<br>',
                       'latency': f'{latency}<br>',
                       'memory_footprint_of_ml_model': f'{memory_footprint_of_ml_model}<br>'}
        return json_output

    def get_carbontracker(self, task_id, wait):
        if wait:
            with self.handler_.condition:
                while not self.handler_.results_available(task_id, utils.node_id.CARBONTRACKER.value):
                    self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_carbontracker(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.CARBONTRACKER.value)} data for task {utils.string_task(task_id)}<br>"}

        # Parse data into json
        carbon_footprint = node_data.carbon_footprint()
        energy_consumption = node_data.energy_consumption()
        carbon_intensity = node_data.carbon_intensity()
        json_output = {'carbon_footprint': f'{carbon_footprint}<br>',
                       'energy_consumption': f'{energy_consumption}<br>',
                       'carbon_intensity': f'{carbon_intensity}<br>'}
        return json_output

    def get_results(self, node_id, task_id):
        wait = False
        if task_id is None:
            task_id = self.get_last_task_id()
            wait = True

        if node_id == utils.node_id.APP_REQUIREMENTS.value:
            return self.get_app_requirements(task_id, wait)
        elif node_id == utils.node_id.ML_MODEL_METADATA.value:
            return self.get_model_metadata(task_id, wait)
        elif node_id == utils.node_id.HW_CONSTRAINTS.value:
            return self.get_hw_constraints(task_id, wait)
        elif node_id == utils.node_id.ML_MODEL_PROVIDER.value:
            return self.get_ml_model_provider(task_id, wait)
        elif node_id == utils.node_id.HW_PROVIDER.value:
            return self.get_hw_provider(task_id, wait)
        elif node_id == utils.node_id.CARBONTRACKER.value:
            return self.get_carbontracker(task_id, wait)
        else:
            return utils.string_node(node_id) + " node does not have any results to show.<br>"

    def send_user_input(self, json_data):
        pair = self.node_.prepare_new_task()
        task_id = pair[0]
        user_input = pair[1]
        self.handler_.register_task(task_id)

        user_input.task_id(task_id)
        if (json_data.get('modality') is not None):
            user_input.modality(json_data.get('modality'))
        if (json_data.get('problem_short_description') is not None):
            user_input.problem_short_description(json_data.get('problem_short_description'))
        #user_input.evaluation_metrics(evaluation_metrics)
        #user_input.model(model)
        # TODO add missing fields

        # Prepare extra data
        hw_req = utils.default_hw_requirement
        mem_footprint = utils.default_mem_footprint
        if (json_data.get('hardware_required') is not None):
            hw_req = json_data.get('hardware_required')
        if (json_data.get('max_memory_footprint') is not None):
            mem_footprint = json_data.get('max_memory_footprint')

        # Add extra data to user user_input
        extra_data = {'hardware_required': hw_req,
                      'max_memory_footprint': mem_footprint}
        json_obj = utils.json_dict(extra_data)
        data_array = np.frombuffer(json_obj.encode(), dtype=np.uint8)
        user_input.extra_data(sustainml_swig.uint8_t_vector(data_array.tolist()))
        return self.node_.start_task(task_id, user_input)
