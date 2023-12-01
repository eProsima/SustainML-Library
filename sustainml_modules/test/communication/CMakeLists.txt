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

set(TYPES_SOURCE
    types/typesImpl.cxx
    types/typesImplPubSubTypes.cxx
    )

set(SIMPLE_TASK_PUBSUB_SOURCE ${TYPES_SOURCE}
    SimpleTaskBase.cpp
    SimpleTaskPublisher.cpp
    SimpleTaskSubscriber.cpp
    SimpleTaskPubSubMain.cpp
    )

add_executable(SimplePubSubTask ${SIMPLE_TASK_PUBSUB_SOURCE})
target_link_libraries(SimplePubSubTask
    fastrtps
    fastcdr
    foonathan_memory)

###############################################################################
# Necessary files
###############################################################################
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/chained_nodes_validator.py
    ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py COPYONLY)

if (Python3_FOUND)

    add_test(NAME SimpleCommunicationChainedPyFourNodes
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/chained_nodes_validator.py)

endif()
