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
 * @file Helper.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP

#include <memory>

#include <sustainml_cpp/core/Constants.hpp>
#include <sustainml_cpp/types/types.h>

namespace sustainml {
namespace orchestrator {

/**
 * @brief This helper structure is used to map the NodeID enum class
 * to the associated node data type.
 */
template <NodeID> struct MapToNodeID_t;
template <> struct MapToNodeID_t<NodeID::ID_TASK_ENCODER> { using type = types::EncodedTask; };
template <> struct MapToNodeID_t<NodeID::ID_MACHINE_LEARNING> { using type = types::MLModel; };
template <> struct MapToNodeID_t<NodeID::ID_HARDWARE_RESOURCES> { using type = types::HWResource; };
template <> struct MapToNodeID_t<NodeID::ID_CARBON_FOOTPRINT> { using type = types::CO2Footprint; };

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP
