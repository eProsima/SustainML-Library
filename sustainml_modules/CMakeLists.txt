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

###############################################################################
# CMake build rules for Submodule
###############################################################################
cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME sustainml_modules)

# Done this to set machine architecture and be able to call cmake_utils
enable_language(CXX)

###############################################################################
# Find package cmake_utils
###############################################################################
# Package cmake_utils is required to get every cmake macro needed
find_package(cmake_utils REQUIRED)
find_package(sustainml_cpp REQUIRED)
find_package(cpp_utils REQUIRED)

###############################################################################
# Project
###############################################################################
# Configure project by info set in project_settings.cmake
# - Load project_settings variables
# - Read version
# - Set installation paths
#configure_project()
#
## Call explictly project
#project(
#    ${MODULE_NAME}
#    VERSION
#    ${MODULE_VERSION}
#    DESCRIPTION
#    ${MODULE_DESCRIPTION}
#    LANGUAGES
#    CXX
#)

configure_project_cpp()

# Compile C++ poc executable
add_executable(${MODULE_NAME}_poc src/cpp/modules_poc.cpp)
target_link_libraries(${MODULE_NAME}_poc sustainml_cpp cpp_utils)
