// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BLACKBOX_BLACKBOX_HPP__
#define __BLACKBOX_BLACKBOX_HPP__

#define TEST_TOPIC_NAME std::string( \
        ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name() + std::string( \
            "_") + ::testing::UnitTest::GetInstance()->current_test_info()->name())

#if defined(__cplusplus_winrt)
#define GET_PID GetCurrentProcessId
#include <process.h>
#elif defined(_WIN32)
#define GET_PID _getpid
#include <process.h>
#else
#define GET_PID getpid
#include <sys/types.h>
#include <unistd.h>
#endif // if defined(_WIN32)

#include "common/Common.hpp"

#include "../api/ManagedNode.hpp"
#include "../api/TaskInjector.hpp"

#include <sustainml_cpp/types/types.h>
#include <types/typesImplPubSubTypes.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <list>

#include <gtest/gtest.h>

using namespace sustainml;

/****** Auxiliary data generators *******/
std::list<UserInputImpl> default_userinput_task_generator(
        size_t max = 0);

std::list<MLModelMetadataImpl> default_modelmetadata_task_generator(
        size_t max = 0);

std::list<MLModelImpl> default_mlmodel_task_generator(
        size_t max = 0);

std::list<HWResourceImpl> default_hwresource_task_generator(
        size_t max = 0);

std::list<CO2FootprintImpl> default_carbonfootprint_data_generator(
        size_t max = 0);

/******* Auxiliary Managed Nodes aliases *****/

using MLModelMetadataManagedNode = ManagedNode<ml_model_metadata_module::MLModelMetadataNode,
                ml_model_metadata_module::MLModelMetadataTaskListener,
                types::UserInput, types::NodeStatus, types::MLModelMetadata>;

using MLModelManagedNode = ManagedNode<ml_model_provider_module::MLModelNode,
                ml_model_provider_module::MLModelTaskListener,
                types::MLModelMetadata, types::NodeStatus, types::MLModel>;

using HWResourcesManagedNode = ManagedNode<hardware_module::HardwareResourcesNode,
                hardware_module::HardwareResourcesTaskListener,
                types::MLModel, types::NodeStatus, types::HWResource>;

using CarbonFootprintManagedNode = ManagedNode<carbon_tracker_module::CarbonFootprintNode,
                carbon_tracker_module::CarbonFootprintTaskListener,
                types::MLModel, types::UserInput, types::HWResource,
                types::NodeStatus, types::CO2Footprint>;

/******* Auxiliary Signature aliases *****/
using MLModelMetadataCallbackSignature = std::function<void (
                    types::UserInput& user_input,
                    types::NodeStatus& status,
                    types::MLModelMetadata& output)>;

using MLModelCallbackSignature = std::function<void (
                    types::MLModelMetadata& ml_model_metadata,
                    types::NodeStatus& status,
                    types::MLModel& output)>;

using HWResourcesCallbackSignature = std::function<void (
                    types::MLModel& model,
                    types::NodeStatus& status,
                    types::HWResource& output)>;

using CarbonFootprintCallbackSignature = std::function<void (
                    types::MLModel& model,
                    types::UserInput& user_input,
                    types::HWResource& hardware_resources,
                    types::NodeStatus& status,
                    types::CO2Footprint& output)>;


#endif // __BLACKBOX_BLACKBOX_HPP__
