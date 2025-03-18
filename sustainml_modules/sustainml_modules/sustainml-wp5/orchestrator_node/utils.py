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
"""SustainML Orchestrator Node utility methods."""

from enum import Enum
import json

default_hw_requirement = "PIM_AI_1chip"
default_mem_footprint = 100

class node_id(Enum):
    APP_REQUIREMENTS = 0
    CARBONTRACKER = 1
    HW_CONSTRAINTS = 2
    HW_PROVIDER = 3
    ML_MODEL_METADATA = 4
    ML_MODEL_PROVIDER = 5
    # Proxies end here since the MAX is used to allocate proxy resources in the orchestrator
    MAX = 6
    ORCHESTRATOR = 7
    UNKNOWN = 8

class node_status(Enum):
    INACTIVE = 0
    ERROR = 1
    IDLE = 2
    INITIALIZING = 3
    RUNNING = 4
    TERMINATING = 5
    UNKNOWN = 6

# Node status from integer to string
def string_status(status):
    if status == node_status.INACTIVE.value:          # NODE_INACTIVE
        return "INACTIVE"
    elif status == node_status.ERROR.value:           # NODE_ERROR
        return "ERROR"
    elif status == node_status.IDLE.value:            # NODE_IDLE
        return "IDLE"
    elif status == node_status.INITIALIZING.value:    # NODE_INITIALIZING
        return "INITIALIZING"
    elif status == node_status.RUNNING.value:         # NODE_RUNNING
        return "RUNNING"
    elif status == node_status.TERMINATING.value:     # NODE_TERMINATING
        return "TERMINATING"
    else:
        return "Unknown status"

def string_node(node):
    if node == node_id.APP_REQUIREMENTS.value:        # ID_APP_REQUIREMENTS
        return "APP_REQUIREMENTS"
    elif node == node_id.CARBONTRACKER.value:         # ID_CARBON_FOOTPRINT
        return "CARBON_FOOTPRINT"
    elif node == node_id.HW_CONSTRAINTS.value:        # ID_HW_CONSTRAINTS
        return "HW_CONSTRAINTS"
    elif node == node_id.HW_PROVIDER.value:           # ID_HW_RESOURCES
        return "HW_RESOURCES"
    elif node == node_id.ML_MODEL_METADATA.value:     # ID_ML_MODEL_METADATA
        return "ML_MODEL_METADATA"
    elif node == node_id.ML_MODEL_PROVIDER.value:     # ID_ML_MODEL
        return "ML_MODEL"
    elif node == node_id.MAX.value:                   # MAX
        return "MAX"
    elif node == node_id.ORCHESTRATOR.value:          # ID_ORCHESTRATOR
        return "ORCHESTRATOR"
    else:
        return "UNKNOWN"

def string_std_vector(vector):
    output = ""
    vector_size = 0
    if vector is not None:
        vector_size = vector.size()
    for i in range(vector_size):
        if output != "":
            output += ", "
        output += str(vector[i])
    return output

def task_json(task_id):
    return {"problem_id": task_id.problem_id(), "iteration_id": task_id.iteration_id()}

def string_task(task):
    return "{" + str(task.problem_id()) + ", " + str(task.iteration_id()) + "}"

def json_dict(dict):
    return json.dumps(dict, indent=4)

def dict_from_json(json_obj):
    return json.loads(json_obj)

def response_json(response):
    return {
        "node_id": response.node_id(),
        "transaction_id": response.transaction_id(),
        "success": response.success(),
        "err_code": response.err_code(),
        "configuration": response.configuration()
    }
