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


#ifndef SUSTAINML_CPP__TASKENCODERNODE_HPP
#define SUSTAINML_CPP__TASKENCODERNODE_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/core/QueuedNodeListener.hpp>
#include <sustainml_cpp/core/Callable.hpp>
#include <types/types.h>

#include <vector>
#include <memory>

namespace sustainml {
namespace task_encoding {

    class Node;
    class Dispatcher;

    class TaskEncoderNode : public Callable<UserInput, NodeStatus, EncodedTask>,
                            public ::sustainml::Node
    {

    public:

        TaskEncoderNode();

        ~TaskEncoderNode();

    private:

        void publish_to_user(const std::vector<void*> inputs) override;

        std::unique_ptr<Dispatcher> dispatcher_;

        std::unique_ptr<QueuedNodeListener<UserInput>> listener_enc_task_queue_;

        //! task id to <NodeStatus, EncodedTask>
        std::map<int, std::pair<NodeStatus, EncodedTask>>  user_data_;

    };

} // namespace task_encoding
} // namespace sustainml

#endif // SUSTAINML_CPP__TASKENCODERNODE_HPP
