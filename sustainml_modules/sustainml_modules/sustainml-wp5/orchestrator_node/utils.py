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
from sustainml_swig import AppRequirements, CO2Footprint, HWConstraints, HWResource, MLModelMetadata, MLModel
import sustainml_swig

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
        return "Application-level requirements"
    elif node == node_id.CARBONTRACKER.value:         # ID_CARBON_FOOTPRINT
        return "Carbontracker"
    elif node == node_id.HW_CONSTRAINTS.value:        # ID_HW_CONSTRAINTS
        return "HW Constraints for inference"
    elif node == node_id.HW_PROVIDER.value:           # ID_HW_RESOURCES
        return "HW Provider"
    elif node == node_id.ML_MODEL_METADATA.value:     # ID_ML_MODEL_METADATA
        return "ML Model Metadata"
    elif node == node_id.ML_MODEL_PROVIDER.value:     # ID_ML_MODEL
        return "ML Model Provider"
    elif node == node_id.ORCHESTRATOR.value:          # ID_ORCHESTRATOR (MAX is ID 6)
        return "Orchestrator"
    else:
        return "Unknown node"

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

def string_task(task):
    return "{" + str(task.problem_id()) + ", " + str(task.iteration_id()) + "}"

