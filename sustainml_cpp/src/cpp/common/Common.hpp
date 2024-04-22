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
 * @file Common.h
 */

#ifndef SUSTAINMLCPP_COMMON_COMMON_HPP
#define SUSTAINMLCPP_COMMON_COMMON_HPP

#include <sustainml_cpp/core/Constants.hpp>
#include <types/types.h>

#include <fastrtps/log/Log.h>
#include <fastrtps/utils/fixed_size_string.hpp>

#include <iostream>
#include <map>
#include <utility>

namespace sustainml {
namespace common {

constexpr int INVALID_ID = 0;

//!Node Names
constexpr const char* APP_REQUIREMENTS_NODE = "APP_REQUIREMENTS_NODE";
constexpr const char* CARBON_FOOTPRINT_NODE = "CARBON_FOOTPRINT_NODE";
constexpr const char* HW_CONSTRAINTS_NODE = "HW_CONSTRAINTS_NODE";
constexpr const char* HW_RESOURCES_NODE = "HW_RESOURCES_NODE";
constexpr const char* ML_MODEL_METADATA_NODE = "ML_MODEL_METADATA_NODE";
constexpr const char* ML_MODEL_NODE = "ML_MODEL_NODE";

inline NodeID get_node_id_from_name(
        const eprosima::fastrtps::string_255& name)
{
    NodeID id = NodeID::UNKNOWN;

    if (name == APP_REQUIREMENTS_NODE)
    {
        id = NodeID::ID_APP_REQUIREMENTS;
    }
    else if (name == CARBON_FOOTPRINT_NODE)
    {
        id = NodeID::ID_CARBON_FOOTPRINT;
    }
    else if (name == HW_CONSTRAINTS_NODE)
    {
        id = NodeID::ID_HW_CONSTRAINTS;
    }
    else if (name == HW_RESOURCES_NODE)
    {
        id = NodeID::ID_HW_RESOURCES;
    }
    else if (name == ML_MODEL_METADATA_NODE)
    {
        id = NodeID::ID_ML_MODEL_METADATA;
    }
    else if (name == ML_MODEL_NODE)
    {
        id = NodeID::ID_ML_MODEL;
    }
    return id;
}

enum Topics
{
    NODE_CONTROL,
    NODE_STATUS,
    APP_REQUIREMENT,
    CARBON_FOOTPRINT,
    HW_CONSTRAINT,
    HW_RESOURCE,
    ML_MODEL_METADATA,
    ML_MODEL,
    USER_INPUT,
    HW_RESOURCES_BASELINE,
    ML_MODEL_BASELINE,
    CARBON_FOOTPRINT_BASELINE,
    MAX
};

inline Topics get_topic_from_name(
        const char* name,
        bool baseline)
{
    Topics output = Topics::MAX;

    if (name == APP_REQUIREMENTS_NODE)
    {
        output = Topics::APP_REQUIREMENT;
    }
    else if (name == CARBON_FOOTPRINT_NODE)
    {
        if (baseline)
        {
            output = Topics::CARBON_FOOTPRINT_BASELINE;
        }
        else
        {
            output = Topics::CARBON_FOOTPRINT;
        }
    }
    else if (name == HW_CONSTRAINTS_NODE)
    {
        output = Topics::HW_CONSTRAINT;
    }
    else if (name == HW_RESOURCES_NODE)
    {
        if (baseline)
        {
            output = Topics::HW_RESOURCES_BASELINE;
        }
        else
        {
            output = Topics::HW_RESOURCE;
        }
    }
    if (name == ML_MODEL_METADATA_NODE)
    {
        output = Topics::ML_MODEL_METADATA;
    }
    else if (name == ML_MODEL_NODE)
    {
        if (baseline)
        {
            output = Topics::ML_MODEL_BASELINE;
        }
        else
        {
            output = Topics::ML_MODEL;
        }
    }
    return output;
}

/*!
 * @brief Map in which to store all the topics, name and typename
 */
class TopicCollection
{
public:

    static std::map<Topics, std::pair<std::string, std::string>>& get()
    {
        static std::map<Topics, std::pair<std::string, std::string>>  topics {
            {NODE_CONTROL, {"/sustainml/control", "NodeControlImpl"}},
            {NODE_STATUS, {"/sustainml/status", "NodeStatusImpl"}},
            {APP_REQUIREMENT, {"/sustainml/app_requirements/output", "AppRequirementsImpl"}},
            {CARBON_FOOTPRINT, {"/sustainml/carbon_tracker/output", "CO2FootprintImpl"}},
            {HW_CONSTRAINT, {"/sustainml/hw_constraints/output", "HWConstraintsImpl"}},
            {HW_RESOURCE, {"/sustainml/hw_resources/output", "HWResourceImpl"}},
            {ML_MODEL_METADATA, {"/sustainml/ml_model_metadata/output", "MLModelMetadataImpl"}},
            {ML_MODEL, {"/sustainml/ml_model_provider/output", "MLModelImpl"}},
            {USER_INPUT, {"/sustainml/user_input", "UserInputImpl"}},
            {ML_MODEL_BASELINE, {"/sustainml/ml_model_provider/baseline", "MLModelImpl"}},
            {HW_RESOURCES_BASELINE, {"/sustainml/hw_resources/baseline", "HWResourceImpl"}},
            {CARBON_FOOTPRINT_BASELINE, {"/sustainml/carbon_tracker/baseline", "CO2FootprintImpl"}}
        };

        return topics;
    }

protected:

    TopicCollection() = default;
};


enum QueueIds
{
    APP_REQUIREMENT_QUEUE,
    CARBON_FOOTPRINT_QUEUE,
    HW_RESOURCE_QUEUE,
    HW_CONSTRAINT_QUEUE,
    ML_MODEL_METADATA_QUEUE,
    ML_MODEL_QUEUE,
    USER_INPUT_QUEUE,
};

inline int queue_name_to_id(
        const std::string& queue_name)
{
    auto tp = TopicCollection::get();

    if (queue_name.find(tp[APP_REQUIREMENT].second) != std::string::npos)
    {
        return APP_REQUIREMENT_QUEUE;
    }
    else if (queue_name.find(tp[CARBON_FOOTPRINT].second) != std::string::npos)
    {
        return CARBON_FOOTPRINT_QUEUE;
    }
    else if (queue_name.find(tp[HW_CONSTRAINT].second) != std::string::npos)
    {
        return HW_CONSTRAINT_QUEUE;
    }
    else if (queue_name.find(tp[HW_RESOURCE].second) != std::string::npos)
    {
        return HW_RESOURCE_QUEUE;
    }
    else if (queue_name.find(tp[ML_MODEL_METADATA].second) != std::string::npos)
    {
        return ML_MODEL_METADATA_QUEUE;
    }
    else if (queue_name.find(tp[ML_MODEL].second) != std::string::npos)
    {
        return ML_MODEL_QUEUE;
    }
    else if (queue_name.find(tp[USER_INPUT].second) != std::string::npos)
    {
        return USER_INPUT_QUEUE;
    }
    else
    {
        EPROSIMA_LOG_ERROR(COMMON, "Could not find the requested queue type name " << queue_name);
    }

    return -1;
}

template<typename T>
inline int sample_type_to_queue_id(
        T* sample)
{
    if (typeid(sample) == typeid(types::AppRequirements*))
    {
        return APP_REQUIREMENT_QUEUE;
    }
    else if (typeid(sample) == typeid(types::CO2Footprint*))
    {
        return CARBON_FOOTPRINT_QUEUE;
    }
    else if (typeid(sample) == typeid(types::HWConstraints*))
    {
        return HW_CONSTRAINT_QUEUE;
    }
    else if (typeid(sample) == typeid(types::HWResource*))
    {
        return HW_RESOURCE_QUEUE;
    }
    else if (typeid(sample) == typeid(types::MLModelMetadata*))
    {
        return ML_MODEL_METADATA_QUEUE;
    }
    else if (typeid(sample) == typeid(types::MLModel*))
    {
        return ML_MODEL_QUEUE;
    }
    else if (typeid(sample) == typeid(types::UserInput*))
    {
        return USER_INPUT_QUEUE;
    }

    return -1;
}

template<typename T>
inline bool pair_queue_id_with_sample_type(
        std::vector<std::pair<int, void*>> input_samples,
        T*& sample,
        const size_t& expected_samples,
        size_t& n_samples_retrieved)
{
    bool ret = false;

    for (std::pair<int, void*>& input : input_samples)
    {
        if (sample_type_to_queue_id(sample) == input.first)
        {
            sample = reinterpret_cast<decltype(sample)>(input.second);
            ++n_samples_retrieved;
            ret = true;
        }
    }

    return ret;
}

template<std::size_t I = 0, typename ... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
pair_queue_id_with_sample_type(
        const std::vector<std::pair<int, void*>>& input_samples,
        std::tuple<Tp...>& t,
        const size_t& expected_samples,
        size_t& n_samples_retrieved)
{
}

template<std::size_t I = 0, typename ... Tp>
inline typename std::enable_if < I < sizeof...(Tp), void>::type
pair_queue_id_with_sample_type(
        const std::vector<std::pair<int, void*>>& input_samples,
        std::tuple<Tp...>& t_args,
        const size_t& expected_samples,
        size_t& n_samples_retrieved)
{

    if (!pair_queue_id_with_sample_type(
                input_samples,
                std::get<I>(t_args),
                expected_samples,
                n_samples_retrieved))
    {
        if (n_samples_retrieved != expected_samples)
        {
            EPROSIMA_LOG_ERROR(COMMON, "Expected sample type not found in retrieved samples");
        }

        return;
    }
    pair_queue_id_with_sample_type<I + 1, Tp...>(
        input_samples,
        t_args,
        expected_samples,
        n_samples_retrieved);
}

} //common
} //sustainml

#endif //SUSTAINMLCPP_COMMON_COMMON_HPP
