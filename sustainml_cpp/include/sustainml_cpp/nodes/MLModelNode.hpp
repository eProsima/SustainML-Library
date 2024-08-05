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

/**
 * @file MLModelNode.hpp
 */


#ifndef SUSTAINMLCPP_NODES_MLMODELNODE_HPP
#define SUSTAINMLCPP_NODES_MLMODELNODE_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/core/Callable.hpp>
#include <sustainml_cpp/types/types.h>

#include <vector>
#include <memory>

namespace sustainml {
namespace core {
template<class T> class QueuedNodeListener;
} // namespace core

namespace utils {
template<class T> class SamplePool;
} // namespace utils

namespace ml_model_module {

class Node;
class Dispatcher;

using MLModelCallable = core::Callable<types::MLModelMetadata, types::AppRequirements, types::HWConstraints,
                types::MLModel, types::HWResource, types::CO2Footprint, types::NodeStatus, types::MLModel>;

struct MLModelTaskListener : public MLModelCallable
{
    virtual ~MLModelTaskListener()
    {
    }

    virtual void on_new_task_available(
            types::MLModelMetadata& model_metadata,
            types::AppRequirements& requirements,
            types::HWConstraints& constraints,
            types::MLModel& ml_model_baseline,
            types::HWResource& hw_baseline,
            types::CO2Footprint& carbonf_baseline,
            types::NodeStatus& status,
            types::MLModel& output) override
    {
    }

};

/**
 * @brief Machine Learning Model Node Implementation
 * It requires the
 * - ML model metadata
 * as input
 */
class MLModelNode : public ::sustainml::core::Node
{
    enum ExpectedInputSamples
    {
        ML_MODEL_METADATA_SAMPLE,
        APP_REQUIREMENTS_SAMPLE,
        HW_CONSTRAINTS_SAMPLE,
        ML_MODEL_BASELINE_SAMPLE,
        HW_RESOURCE_BASELINE_SAMPLE,
        CARBON_FOOTPRINT_BASELINE_SAMPLE,
        MAX
    };

    enum TaskData
    {
        TASK_STATUS_DATA = ExpectedInputSamples::MAX,
        TASK_OUTPUT_DATA
    };

public:

    SUSTAINML_CPP_DLL_API MLModelNode(
            MLModelTaskListener& user_listener);

#ifndef SWIG_WRAPPER
    SUSTAINML_CPP_DLL_API MLModelNode(
            MLModelTaskListener& user_listener,
            sustainml::core::Options opts);
#endif // SWIG_WRAPPER

    SUSTAINML_CPP_DLL_API virtual ~MLModelNode();

private:

    /**
     * @brief Initialize the DDS entities contained in the Node
     *
     * @param opts opts Options object with the QoS configuration
     */
    void init(
            const sustainml::core::Options& opts);

    /**
     * @brief Invokes the user callback with the provided inputs.
     *
     * @param inputs A vector containing the required samples. All the samples
     * must correspond to the same task_id.
     */
    void publish_to_user(
            const types::TaskId& task_id,
            const std::vector<std::pair<int, void*>> inputs) override;

    MLModelTaskListener& user_listener_;

    std::unique_ptr<core::QueuedNodeListener<types::MLModelMetadata>> listener_model_metadata_queue_;
    std::unique_ptr<core::QueuedNodeListener<types::AppRequirements>> listener_app_requirements_queue_;
    std::unique_ptr<core::QueuedNodeListener<types::HWConstraints>> listener_hw_constraints_queue_;

    // Baseline topics
    std::unique_ptr<core::QueuedNodeListener<types::MLModel>> listener_mlmodel_queue_;
    std::unique_ptr<core::QueuedNodeListener<types::HWResource>> listener_hw_queue_;
    std::unique_ptr<core::QueuedNodeListener<types::CO2Footprint>> listener_carbon_footprint_queue_;

    std::mutex mtx_;

    std::unique_ptr<utils::SamplePool<types::NodeTaskOutputData<types::MLModel>>> task_data_pool_;

};

} // namespace ml_model_module
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_MLMODELNODE_HPP
