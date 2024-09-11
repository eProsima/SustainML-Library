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

find_package(sustainml_cpp REQUIRED)
find_package(fastdds 3 REQUIRED)

include_directories(${sustainml_cpp_INCLUDE_DIR})

set(TYPES_SOURCE
    types/typesImplPubSubTypes.cxx
    types/typesImplTypeObjectSupport.cxx
    )

set(SIMPLE_TASK_PUBSUB_SOURCE ${TYPES_SOURCE}
    SimpleTaskBase.cpp
    SimpleTaskPublisher.cpp
    SimpleTaskSubscriber.cpp
    SimpleTaskPubSubMain.cpp
    )

add_executable(SimplePubSubTask ${SIMPLE_TASK_PUBSUB_SOURCE})
target_link_libraries(SimplePubSubTask
    fastdds
    fastcdr
    foonathan_memory)

add_executable(SimpleOrchestratorNode SimpleOrchestratorNode.cpp)
target_link_libraries(SimpleOrchestratorNode
    fastdds
    fastcdr
    foonathan_memory
    sustainml_cpp)

###############################################################################
# Necessary files
###############################################################################
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/chained_nodes_validator.py
    ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py COPYONLY)

if (Python3_FOUND)

    add_test(NAME SimpleCommunicationSixNodesChainedPython
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py
            -ps SimplePubSubTask
            -tp /sustainml/user_input
            -ts /sustainml/carbon_tracker/output
            --baseline-topics /sustainml/ml_model_provider/baseline /sustainml/hw_resources/baseline /sustainml/carbon_tracker/baseline
            --baseline-topic-types ml hw co2
            -ttp ui
            -tts co2
            --samples 1
            -mlm ml_model_metadata_node.py
            -ml ml_model_provider_node.py
            -hw hw_resources_provider_node.py
            -co2 carbon_footprint_node.py
            -hwc hw_constraints_node.py
            -ap app_requirements_node.py)

    add_test(NAME SimpleCommunicationOrchestratorSixNodesPython
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py
            -mlm ml_model_metadata_node.py
            -ml ml_model_provider_node.py
            -hw hw_resources_provider_node.py
            -co2 carbon_footprint_node.py
            -hwc hw_constraints_node.py
            -ap app_requirements_node.py
            -orc SimpleOrchestratorNode)

endif()
