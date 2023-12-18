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

# Done this to set machine architecture and be able to call cmake_utils
enable_language(CXX)

###############################################################################
# Find package cmake_utils
###############################################################################
# Package cmake_utils is required to get every cmake macro needed
find_package(cmake_utils REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development)

###############################################################################
# Project
###############################################################################
# Configure project by info set in project_settings.cmake
# - Load project_settings variables
# - Read version
# - Set installation paths
configure_project()

# Call explictly project
project(
    ${MODULE_NAME}
    VERSION
        ${MODULE_VERSION}
    DESCRIPTION
        ${MODULE_DESCRIPTION}
    LANGUAGES
        CXX
)

###############################################################################
# C++ Project
###############################################################################
# Configure CPP project for dependencies and required flags:
# - Set CMake Build Type
# - Set C++ version
# - Set shared libraries by default
# - Find external packages and thirdparties
# - Activate Code coverage if flag CODE_COVERAGE
# - Activate Address sanitizer if flag ASAN_BUILD
# - Configure log depending on LOG_INFO flag and CMake type
configure_project_cpp()

###############################################################################
# C++ Project
###############################################################################
# Update submodules
message(STATUS "Updating submodules")
execute_process(
    COMMAND git submodule update --recursive --init
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE EXECUTE_RESULT
    )

###############################################################################
# Test
###############################################################################
# Compile tests if CMake options requires it
compile_test_library(
    "${PROJECT_SOURCE_DIR}/test" # Test directory
)

###############################################################################
# Packaging
###############################################################################
# Install package
eprosima_packaging()


###############################################################################
# Setup Python Modules
###############################################################################
if (Python3_FOUND)

    # Build
    add_custom_target(
        cmake_python_build_${package_name} ALL
        COMMAND ${Python3_EXECUTABLE} setup.py build --build-base ${PROJECT_BINARY_DIR} -f
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      )

    # Copy to Build ...
    add_custom_target(
        cmake_python_copy_${package_name}_setup ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/setup.py ${PROJECT_BINARY_DIR}
        DEPENDS cmake_python_build_${package_name}
      )

    add_custom_target(
        cmake_python_copy_${package_name}_readme ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${PROJECT_BINARY_DIR} &&
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${PROJECT_BINARY_DIR}/lib
        DEPENDS cmake_python_copy_${package_name}_setup
      )

    add_custom_target(
        cmake_python_copy_${package_name}_resource ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/lib/resource &&
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resource/${PROJECT_NAME}
        ${PROJECT_BINARY_DIR}/lib/resource
        DEPENDS cmake_python_copy_${package_name}_readme
      )

    add_custom_target(
        cmake_python_copy_${package_name}_package ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/package.xml ${PROJECT_BINARY_DIR}/lib
        DEPENDS cmake_python_copy_${package_name}_resource
      )

    # Install data files
    add_custom_target(
        cmake_install_data_files ALL
        COMMAND ${Python3_EXECUTABLE} ../setup.py install_data
        --install-dir ${CMAKE_INSTALL_PREFIX}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        DEPENDS cmake_python_copy_${package_name}_package
    )

    # Install egg
    add_custom_target(
        cmake_install_python_egg ALL
        COMMAND ${Python3_EXECUTABLE} ../setup.py install_egg_info
        --install-dir ${CMAKE_INSTALL_PREFIX}/lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        DEPENDS cmake_python_copy_${package_name}_resource
    )

    # Install lib
    add_custom_target(
        cmake_install_python_lib ALL
        COMMAND ${Python3_EXECUTABLE} ../setup.py install_lib
        --install-dir ${CMAKE_INSTALL_PREFIX}/lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        DEPENDS cmake_install_python_egg
    )

else()

    message(ERROR "Python3 package needed")

endif()
