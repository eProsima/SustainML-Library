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
import json

class OrchestratorNodeHandle(cpp_OrchestratorNodeHandle):

    def __init__(self, orchestrator):
        super().__init__()
        self.orchestrator = orchestrator
        self.condition = threading.Condition()
        self.last_task_id = None
        self.node_status_ = {}
        self.result_status = {}

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
            id: int,
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

            # If the node is CarbonTracker and its output extra_data is non-empty, print a message
            if node_id == utils.node_id.CARBONTRACKER.value:
                carbon_data = sustainml_swig.get_carbontracker_task_data(self.orchestrator.node_, task_id)
                try:
                    extra_data_vector = carbon_data.extra_data()  # Vector de uint8_t
                    extra_data_list = [s for s in extra_data_vector]
                    extra_data_bytes = bytes(extra_data_list)
                    extra_data_str = extra_data_bytes.decode('utf-8')
                    extra_data = json.loads(extra_data_str)
                except AttributeError:
                    extra_data = None

                if extra_data is not None and len(extra_data) > 0:
                    num_outputs = extra_data['num_outputs']

                    if num_outputs > 1:
                        print(f"Reiterating for multiple outputs")
                        user_json = self.orchestrator.get_user_input_data(task_id)
                        user_json.get('extra_data', {})['num_outputs'] = num_outputs - 1
                        user_json['previous_iteration'] = task_id.iteration_id()
                        user_json.get('extra_data', {})['previous_problem_id'] = task_id.problem_id()
                        user_json.get('extra_data', {})['model_restrains'] = extra_data['model_restrains']
                        self.orchestrator.send_user_input(user_json)

    def results_available(self, task_id, node_id):
        with self.condition:
            return self.result_status[utils.string_task(task_id)].get(node_id, False)

class Orchestrator:

    def __init__(self):

        self.handler_ = OrchestratorNodeHandle(self)
        self.node_ = cpp_OrchestratorNode(self.handler_)
        self._txn_lock = threading.Lock()
        self._txn_counter = 0

    # Proxy method to run the node
    def run(self):

        self.node_.spin()

    # Proxy method to manually terminate
    def terminate(self):

        self.node_.terminate()

    def get_last_task_id(self):
        return self.handler_.last_task_id

    def get_all_status(self):
        json_output = {}
        for key, value in self.handler_.node_status_.items():
            json_output[utils.string_node(key)] = utils.string_status(value)
        return json_output

    def get_status(self, node_id):
        if node_id is None:
            return self.get_all_status()
        else:
            if node_id in self.handler_.node_status_:
                return utils.string_status(self.handler_.node_status_[node_id])
            else:
                return utils.string_status(utils.node_status.INACTIVE.value)

    def get_app_requirements_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.APP_REQUIREMENTS.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_app_requirements_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.APP_REQUIREMENTS.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        app_requirements_str_list = node_data.app_requirements()
        json_output = {'task_id': task_json,
                       'app_requirements': utils.string_std_vector(app_requirements_str_list)}
        return json_output

    def get_model_metadata_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.ML_MODEL_METADATA.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_model_metadata_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_METADATA.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        keywords_str_list = node_data.keywords()
        metadata_str_list = node_data.ml_model_metadata()
        json_output = {'task_id': task_json,
                       'keywords': utils.string_std_vector(keywords_str_list),
                       'metadata': utils.string_std_vector(metadata_str_list)}
        return json_output

    def get_hw_constraints_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.HW_CONSTRAINTS.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_hw_constraints_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_CONSTRAINTS.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        max_value = node_data.max_memory_footprint()
        required_hardware = node_data.hardware_required()
        json_output = {'task_id': task_json,
                       'max_memory_footprint': max_value,
                       'hardware_required': utils.string_std_vector(required_hardware)}
        return json_output

    def get_model_provider_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.ML_MODEL_PROVIDER.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_model_provider_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ML_MODEL_PROVIDER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        model = node_data.model()
        model_path = node_data.model_path()
        model_properties = node_data.model_properties()
        model_properties_path = node_data.model_properties_path()
        input_batch = node_data.input_batch()
        target_latency = node_data.target_latency()
        json_output = {'task_id': task_json,
                       'model': model,
                       'model_path': model_path,
                       'model_properties': model_properties,
                       'model_properties_path': model_properties_path,
                       'input_batch': utils.string_std_vector(input_batch),
                       'target_latency': target_latency}
        return json_output

    def get_hw_provider_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.HW_PROVIDER.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_hw_provider_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.HW_PROVIDER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        hw_description = node_data.hw_description()
        power_consumption = node_data.power_consumption()
        latency = node_data.latency()
        memory_footprint_of_ml_model = node_data.memory_footprint_of_ml_model()
        json_output = {'task_id': task_json,
                       'hw_description': hw_description,
                       'power_consumption': power_consumption,
                       'latency': latency,
                       'memory_footprint_of_ml_model': memory_footprint_of_ml_model}
        return json_output

    def get_carbontracker_task_data(self, task_id):
        with self.handler_.condition:
            while not self.handler_.results_available(task_id, utils.node_id.CARBONTRACKER.value):
                self.handler_.condition.wait()

        # retrieve node data
        node_data = sustainml_swig.get_carbontracker_task_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.CARBONTRACKER.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        carbon_footprint = node_data.carbon_footprint()
        energy_consumption = node_data.energy_consumption()
        carbon_intensity = node_data.carbon_intensity()
        extra_data_vector = node_data.extra_data()
        extra_data_list = [s for s in extra_data_vector]
        extra_data_bytes = bytes(extra_data_list)
        extra_data_str = extra_data_bytes.decode('utf-8')
        extra_data = json.loads(extra_data_str)
        json_output = {'task_id': task_json,
                       'carbon_footprint': carbon_footprint,
                       'energy_consumption': energy_consumption,
                       'carbon_intensity': carbon_intensity,
                       'extra_data': extra_data}

        return json_output

    def get_user_input_data(self, task_id):

        # retrieve node data
        node_data = sustainml_swig.get_user_input_data(self.node_, task_id)
        if node_data is None:
            return {'Error': f"Failed to get {utils.string_node(utils.node_id.ORCHESTRATOR.value)} data for task {utils.string_task(task_id)}"}

        # Parse data into json
        task_json = {'problem_id': task_id.problem_id(), 'iteration_id': task_id.iteration_id()}
        modality = node_data.modality()
        problem_short_description = node_data.problem_short_description()
        problem_definition = node_data.problem_definition()
        inputs = node_data.inputs()
        outputs = node_data.outputs()
        minimum_samples = node_data.minimum_samples()
        maximum_samples = node_data.maximum_samples()
        optimize_carbon_footprint_manual = node_data.optimize_carbon_footprint_manual()
        previous_iteration = node_data.previous_iteration()
        optimize_carbon_footprint_auto = node_data.optimize_carbon_footprint_auto()
        desired_carbon_footprint = node_data.desired_carbon_footprint()
        geo_location_continent = node_data.geo_location_continent()
        geo_location_region = node_data.geo_location_region()
        extra_data_vector = node_data.extra_data()
        extra_data_list = [s for s in extra_data_vector]
        extra_data_bytes = bytes(extra_data_list)
        extra_data_str = extra_data_bytes.decode('utf-8')
        extra_data = json.loads(extra_data_str)
        json_output = {'task_id': task_json,
                   'modality': modality,
                   'problem_short_description': problem_short_description,
                   'problem_definition': problem_definition,
                   'inputs': " ".join(inputs),
                   'outputs': " ".join(outputs),
                   'minimum_samples': minimum_samples,
                   'maximum_samples': maximum_samples,
                   'optimize_carbon_footprint_manual': optimize_carbon_footprint_manual,
                   'previous_iteration': previous_iteration,
                   'optimize_carbon_footprint_auto': optimize_carbon_footprint_auto,
                   'desired_carbon_footprint': desired_carbon_footprint,
                   'geo_location_continent': geo_location_continent,
                   'geo_location_region': geo_location_region,
                   'extra_data': extra_data}
        return json_output

    def get_results(self, node_id, task_id):
        if task_id is None:
            task_id = self.get_last_task_id()

        if node_id == utils.node_id.APP_REQUIREMENTS.value:
            return self.get_app_requirements_task_data(task_id)
        elif node_id == utils.node_id.ML_MODEL_METADATA.value:
            return self.get_model_metadata_task_data(task_id)
        elif node_id == utils.node_id.HW_CONSTRAINTS.value:
            return self.get_hw_constraints_task_data(task_id)
        elif node_id == utils.node_id.ML_MODEL_PROVIDER.value:
            return self.get_model_provider_task_data(task_id)
        elif node_id == utils.node_id.HW_PROVIDER.value:
            return self.get_hw_provider_task_data(task_id)
        elif node_id == utils.node_id.CARBONTRACKER.value:
            return self.get_carbontracker_task_data(task_id)
        elif node_id == utils.node_id.ORCHESTRATOR.value:
            return self.get_user_input_data(task_id)
        else:
            message = utils.string_node(node_id) + " node does not have any results to show."
            return {'message': message, 'task_id': utils.task_json(task_id)}

    def send_user_input(self, json_data):
        if json_data.get('previous_iteration') == 0:
            pair = self.node_.prepare_new_task()
        else:
            previous_task = sustainml_swig.TaskId()
            previous_task.iteration_id(json_data.get('previous_iteration'))
            extra = json_data.get('extra_data', {})
            previous_task.problem_id(extra.get('previous_problem_id'))
            # Verify the last doesn't exist yet
            existing_task = self.get_last_task_id()
            if existing_task is not None:
                if (previous_task.problem_id() == existing_task.problem_id() and
                        previous_task.iteration_id() + 1 <= existing_task.iteration_id()):
                    pair = self.node_.prepare_new_iteration(previous_task, existing_task)
                else:
                    pair = self.node_.prepare_new_iteration(previous_task, previous_task)
            else:
                pair = self.node_.prepare_new_iteration(previous_task, previous_task)
        task_id = pair[0]

        user_input = pair[1]
        self.handler_.register_task(task_id)

        user_input.task_id(task_id)
        if (json_data.get('modality') is not None):
            user_input.modality(json_data.get('modality'))
        if (json_data.get('problem_short_description') is not None):
            user_input.problem_short_description(json_data.get('problem_short_description'))
        if (json_data.get('problem_definition') is not None):
            user_input.problem_definition(json_data.get('problem_definition'))
        if (json_data.get('inputs') is not None):
            user_input.inputs(json_data.get('inputs'))
        if (json_data.get('outputs') is not None):
            user_input.outputs(json_data.get('outputs'))
        if (json_data.get('minimum_samples') is not None):
            user_input.minimum_samples(json_data.get('minimum_samples'))
        if (json_data.get('maximum_samples') is not None):
            user_input.maximum_samples(json_data.get('maximum_samples'))
        if (json_data.get('optimize_carbon_footprint_manual') is not None):
            user_input.optimize_carbon_footprint_manual(json_data.get('optimize_carbon_footprint_manual'))
        if (json_data.get('optimize_carbon_footprint_auto') is not None):
            user_input.optimize_carbon_footprint_auto(json_data.get('optimize_carbon_footprint_auto'))
        if (json_data.get('desired_carbon_footprint') is not None):
            user_input.desired_carbon_footprint(json_data.get('desired_carbon_footprint'))
        if (json_data.get('geo_location_continent') is not None):
            user_input.geo_location_continent(json_data.get('geo_location_continent'))
        if (json_data.get('geo_location_region') is not None):
            user_input.geo_location_region(json_data.get('geo_location_region'))

        #user_input.evaluation_metrics(evaluation_metrics)
        #user_input.model(model)
        # TODO add missing fields

        # Prepare extra data
        extra = json_data.get('extra_data', {})
        hw_req = extra.get('hardware_required', utils.default_hw_requirement)
        mem_footprint = extra.get('max_memory_footprint', utils.default_mem_footprint)
        goal = extra.get('goal')
        model_selected = extra.get('model_selected', None)
        num_outputs = extra.get('num_outputs')
        model_restrains = extra.get('model_restrains', [])
        hf_token = extra.get('hf_token')
        type = extra.get('type')
        dataset_metadata_description = extra.get('dataset_metadata_description', "")
        dataset_metadata_topic = extra.get('dataset_metadata_topic', "")
        dataset_metadata_profile = extra.get('dataset_metadata_profile', "")
        dataset_metadata_keywords = extra.get('dataset_metadata_keywords', "")
        dataset_metadata_applications = extra.get('dataset_metadata_applications', "")

        # Add extra data to user user_input
        extra_data = {'hardware_required': hw_req,
                      'max_memory_footprint': mem_footprint,
                      'goal': goal,
                      'model_selected': model_selected,
                      'num_outputs': num_outputs,
                      'model_restrains': model_restrains,
                      'hf_token': hf_token,
                      'type': type,
                      'dataset_metadata_description': dataset_metadata_description,
                      'dataset_metadata_topic': dataset_metadata_topic,
                      'dataset_metadata_profile': dataset_metadata_profile,
                      'dataset_metadata_keywords': dataset_metadata_keywords,
                      'dataset_metadata_applications': dataset_metadata_applications}
        json_obj = utils.json_dict(extra_data)
        data_array = np.frombuffer(json_obj.encode(), dtype=np.uint8)
        user_input.extra_data(sustainml_swig.uint8_t_vector(data_array.tolist()))

        if self.node_.start_task(task_id, user_input):
            return task_id
        else:
            return None

    def send_request(self, json_data):
        with self._txn_lock:
            self._txn_counter += 1
            txn_id = self._txn_counter

        request_type = sustainml_swig.RequestType()
        request_type.transaction_id(txn_id)
        request_type.configuration(json_data.get('configuration'))

        if "hardwares" in request_type.configuration():
            request_type.node_id(utils.node_id.HW_PROVIDER.value)
        elif any(key in request_type.configuration() for key in ["modality", "in_out_modalities", "metrics", "model_info", "problem_from_modality", "dataset_path"]):
            request_type.node_id(utils.node_id.ML_MODEL_METADATA.value)
        elif any(key in request_type.configuration() for key in ["goal", "model_from_goal"]):
            request_type.node_id(utils.node_id.ML_MODEL_PROVIDER.value)

        if request_type.node_id() is None or request_type.transaction_id() is None or request_type.configuration() is None:
            return None

        res = self.node_.configuration_request(request_type)
        if res.success:
            return res
        else:
            return None
