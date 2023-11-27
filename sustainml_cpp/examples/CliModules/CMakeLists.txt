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

cmake_minimum_required(VERSION 3.5)

project(CliModules VERSION 1 LANGUAGES CXX)

# Find requirements
if(NOT sustainml_cpp_FOUND)
    find_package(sustainml_cpp REQUIRED)
endif()

if(NOT cpp_utils_FOUND)
    find_package(cpp_utils REQUIRED)
endif()

#Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring CliModules example...")
file(GLOB CLI_MODULES_SOURCES_CXX "*.cxx")
file(GLOB CLI_MODULES_SOURCES_CPP "*.cpp")

add_executable(${PROJECT_NAME} ${CLI_MODULES_SOURCES_CXX} ${CLI_MODULES_SOURCES_CPP})

target_link_libraries(${PROJECT_NAME} sustainml_cpp cpp_utils)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION examples/${PROJECT_NAME}/${BIN_INSTALL_DIR})
