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


#ifndef SUSTAINMLCPP_NODES_HARDWARERESOURCESNODE_HPP
#define SUSTAINMLCPP_NODES_HARDWARERESOURCESNODE_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/core/QueuedNodeListener.hpp>
#include <sustainml_cpp/core/Callable.hpp>
#include <types/types.h>

#include <vector>
#include <memory>

namespace sustainml {
namespace hardware_module {

    class Node;
    class Dispatcher;

    /**
    * @brief Hardware Resources Node Implementation
    * It requires the
    * - ML Model
    * as input
    */
    class HardwareResourcesNode : public Callable<MLModel, NodeStatus, HWResource>,
                        public ::sustainml::Node
    {

    public:

        SUSTAINML_CPP_DLL_API HardwareResourcesNode();

        SUSTAINML_CPP_DLL_API ~HardwareResourcesNode();

    private:

        /**
        * @brief Invokes the user callback with the provided inputs.
        *
        * @param inputs A vector containing the required samples. All the samples
        * must correspond to the same task_id.
        */
        void publish_to_user(const std::vector<void*> inputs) override;


        std::unique_ptr<QueuedNodeListener<MLModel>> listener_enc_task_queue_;

        //! task id to <NodeStatus, HWResource>
        std::map<int, std::pair<NodeStatus, HWResource>>  user_data_;

    };

} // namespace hardware_module
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_HARDWARERESOURCESNODE_HPP