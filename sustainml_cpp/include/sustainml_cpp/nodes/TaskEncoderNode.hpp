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
 * @file TaskEncoderNode.hpp
 */


#ifndef SUSTAINMLCPP_NODES_TASKENCODERNODE_HPP
#define SUSTAINMLCPP_NODES_TASKENCODERNODE_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/core/Callable.hpp>
#include <sustainml_cpp/types/types.h>

#include <vector>
#include <memory>

namespace sustainml {
namespace core {
    template<class T> class QueuedNodeListener;
}
namespace ml_task_encoding_module {

    class Node;
    class Dispatcher;

    using TaskEncoderCallable = core::Callable<types::UserInput, types::NodeStatus, types::EncodedTask>;

    struct TaskEncoderTaskListener : public TaskEncoderCallable
    {
        virtual ~TaskEncoderTaskListener()
        {
        }

        virtual void on_new_task_available(
                types::UserInput& user_input,
                types::NodeStatus& status,
                types::EncodedTask& output) override
        {
        }
    };

    /**
    * @brief Task Encoder Node Implementation
    * It requires the
    * - User Input
    * as input
    */
    class TaskEncoderNode : public ::sustainml::core::Node
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

        SUSTAINML_CPP_DLL_API TaskEncoderNode(TaskEncoderTaskListener& user_listener);

        SUSTAINML_CPP_DLL_API TaskEncoderNode(
                sustainml::core::Options opts);

        SUSTAINML_CPP_DLL_API virtual ~TaskEncoderNode();

    private:

        /**
        * @brief Invokes the user callback with the provided inputs.
        *
        * @param inputs A vector containing the required samples. All the samples
        * must correspond to the same task_id.
        */
        void publish_to_user(const std::vector<std::pair<int, void*>> inputs) override;

        TaskEncoderTaskListener& user_listener_;

        std::unique_ptr<core::QueuedNodeListener<types::UserInput>> listener_user_input_queue_;

        std::mutex mtx_;
        //! task id to <NodeStatus, EncodedTask>
        std::map<int, std::pair<types::NodeStatus, types::EncodedTask>>  task_data_;

    };

} // namespace ml_task_encoding_module
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_TASKENCODERNODE_HPP
