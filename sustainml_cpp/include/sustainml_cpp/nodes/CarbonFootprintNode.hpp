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
 * @file CarbonFootprintNode.hpp
 */


#ifndef SUSTAINMLCPP_NODES_CARBONFOOTPRINTNODE_HPP
#define SUSTAINMLCPP_NODES_CARBONFOOTPRINTNODE_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/core/Callable.hpp>
#include <sustainml_cpp/types/types.h>

#include <vector>
#include <memory>

namespace sustainml {
namespace core {
    template<class T> class QueuedNodeListener;
}
namespace co2_tracker_module {

    class Node;
    class Dispatcher;

    /**
    * @brief Carbon Footprint Node Implementation
    * It requires the
    * - User Input
    * - ML Model
    * - HW Resource
    * as inputs
    */

    class CarbonFootprintNode : public core::Callable<MLModel, UserInput, HWResource, NodeStatus, CO2Footprint>,
                                public ::sustainml::core::Node
    {

        enum ExpectedInputs
        {
            ML_MODEL,
            USER_INPUT,
            HW_RESOURCE,
            MAX
        };

    public:

        SUSTAINML_CPP_DLL_API CarbonFootprintNode();

        SUSTAINML_CPP_DLL_API virtual ~CarbonFootprintNode();

    private:

        /**
        * @brief Invokes the user callback with the provided inputs.
        *
        * @param inputs A vector containing the required samples. All the samples
        * must correspond to the same task_id.
        */
        void publish_to_user(const std::vector<std::pair<int, void*>> inputs) override;

        std::unique_ptr<core::QueuedNodeListener<MLModel>> listener_ml_model_queue_;
        std::unique_ptr<core::QueuedNodeListener<UserInput>> listener_user_input_queue_;
        std::unique_ptr<core::QueuedNodeListener<HWResource>> listener_hw_queue_;

        std::mutex mtx_;
        // task id to <NodeStatus, CO2Footprint>
        std::map<int, std::pair<NodeStatus, CO2Footprint>>  user_data_;
    };

} // namespace co2_tracker_module
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_CARBONFOOTPRINTNODE_HPP
