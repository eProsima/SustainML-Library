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
 * @file Helper.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP

#include <memory>

#include <sustainml_cpp/core/Constants.hpp>
#include <sustainml_cpp/types/types.h>

#include <types/typesImpl.hpp>

namespace sustainml {
namespace orchestrator {

/**
 * @brief This helper structure is used to map the NodeID enum class
 * to the associated node data type.
 */
template <NodeID> struct MapFromNodeIDToType_t;
template <> struct MapFromNodeIDToType_t<NodeID::ID_APP_REQUIREMENTS>
{
    using type = types::AppRequirements;
    using typeImpl = AppRequirementsImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_CARBON_FOOTPRINT>
{
    using type = types::CO2Footprint;
    using typeImpl = CO2FootprintImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_HW_CONSTRAINTS>
{
    using type = types::HWConstraints;
    using typeImpl = HWConstraintsImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_HW_RESOURCES>
{
    using type = types::HWResource;
    using typeImpl = HWResourceImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_ML_MODEL>
{
    using type = types::MLModel;
    using typeImpl = MLModelImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_ML_MODEL_METADATA>
{
    using type = types::MLModelMetadata;
    using typeImpl = MLModelMetadataImpl;
};
template <> struct MapFromNodeIDToType_t<NodeID::ID_ORCHESTRATOR>
{
    using type = types::UserInput;
    using typeImpl = UserInputImpl;
};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_HELPER_HPP
