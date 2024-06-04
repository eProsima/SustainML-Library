// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file HardwareConstraintsNode.hpp
 */


#ifndef SUSTAINMLCPP_NODES_HARDWARECONSTRAINTSNODE_HPP
#define SUSTAINMLCPP_NODES_HARDWARECONSTRAINTSNODE_HPP

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

namespace hardware_module {

class Node;
class Dispatcher;

using HardwareConstraintsCallable = core::Callable<types::UserInput, types::NodeStatus, types::HWConstraints>;

struct HardwareConstraintsTaskListener : public HardwareConstraintsCallable
{
    virtual ~HardwareConstraintsTaskListener()
    {
    }

    virtual void on_new_task_available(
            types::UserInput& user_input,
            types::NodeStatus& status,
            types::HWConstraints& output) override
    {
    }

};

/**
 * @brief App Requirements Node Implementation
 * It requires the
 * - User Input
 * as input
 */
class HardwareConstraintsNode : public ::sustainml::core::Node
{

    enum ExpectedInputSamples
    {
        USER_INPUT_SAMPLE,
        MAX
    };

    enum TaskData
    {
        TASK_STATUS_DATA = ExpectedInputSamples::MAX,
        TASK_OUTPUT_DATA
    };

public:

    SUSTAINML_CPP_DLL_API HardwareConstraintsNode(
            HardwareConstraintsTaskListener& user_listener);

#ifndef SWIG_WRAPPER
    SUSTAINML_CPP_DLL_API HardwareConstraintsNode(
            HardwareConstraintsTaskListener& user_listener,
            sustainml::core::Options opts);
#endif // SWIG_WRAPPER

    SUSTAINML_CPP_DLL_API virtual ~HardwareConstraintsNode();

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

    HardwareConstraintsTaskListener& user_listener_;

    std::unique_ptr<core::QueuedNodeListener<types::UserInput>> listener_user_input_queue_;

    std::mutex mtx_;

    std::unique_ptr<utils::SamplePool<types::NodeTaskOutputData<types::HWConstraints>>> task_data_pool_;

};

} // namespace hardware_module
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_HARDWARECONSTRAINTSNODE_HPP
