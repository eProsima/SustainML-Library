# Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SimpleOrchestratorNode.py
    ${CMAKE_CURRENT_BINARY_DIR}/SimpleOrchestratorNode.py COPYONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/RequestOrchestratorNode.py
    ${CMAKE_CURRENT_BINARY_DIR}/RequestOrchestratorNode.py COPYONLY)

if (Python3_FOUND)

    add_test(NAME SimpleCommunicationOrchestratorSixNodesPython
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py
            -mlm ml_model_metadata_node.py
            -ml ml_model_provider_node.py
            -hw hw_resources_provider_node.py
            -co2 carbon_footprint_node.py
            -hwc hw_constraints_node.py
            -ap app_requirements_node.py
            -orc SimpleOrchestratorNode)

    add_test(NAME SimpleCommunicationPythonOrchestratorSixNodesPython
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py
            -mlm ml_model_metadata_node.py
            -ml ml_model_provider_node.py
            -hw hw_resources_provider_node.py
            -co2 carbon_footprint_node.py
            -hwc hw_constraints_node.py
            -ap app_requirements_node.py
            -py-orc SimpleOrchestratorNode.py)

    add_test(NAME SimpleServicePythonOrchestratorSixNodesPython
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py
            -mlm ml_model_metadata_node.py
            -ml ml_model_provider_node.py
            -hw hw_resources_provider_node.py
            -co2 carbon_footprint_node.py
            -hwc hw_constraints_node.py
            -ap app_requirements_node.py
            -py-orc RequestOrchestratorNode.py)

    set_tests_properties(SimpleCommunicationOrchestratorSixNodesPython     PROPERTIES TIMEOUT 500)
    set_tests_properties(SimpleCommunicationPythonOrchestratorSixNodesPython PROPERTIES TIMEOUT 500)
    set_tests_properties(SimpleServicePythonOrchestratorSixNodesPython     PROPERTIES TIMEOUT 500)

endif()
