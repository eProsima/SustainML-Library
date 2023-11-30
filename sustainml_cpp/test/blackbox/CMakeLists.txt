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


file(GLOB BLACKBOXTESTS_TEST_SOURCE "common/BlackboxTests*.cpp")
set(BLACKBOXTESTS_SOURCE ${BLACKBOXTESTS_TEST_SOURCE}
    types/typesImpl.cxx
    types/typesImplPubSubTypes.cxx

    helpers/data_generators.cpp
    )
add_executable(BlackboxTests ${BLACKBOXTESTS_SOURCE})

target_link_libraries(BlackboxTests
    sustainml_cpp
    fastrtps
    fastcdr
    foonathan_memory
    GTest::gtest)

gtest_discover_tests(BlackboxTests)