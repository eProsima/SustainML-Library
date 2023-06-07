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
# Set settings for project sustainml_swig for SWIG code
###############################################################################

set(MODULE_NAME
    sustainml_swig)

set(MODULE_SUMMARY
    "SWIG library to create python bindings from sustainml_cpp.")

set(MODULE_FIND_PACKAGES
        sustainml_cpp
        cpp_utils
    )

set(MODULE_DEPENDENCIES
        ${MODULE_FIND_PACKAGES}
    )
