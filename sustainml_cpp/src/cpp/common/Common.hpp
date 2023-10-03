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

#include <types/types.h>

#include <fastrtps/log/Log.h>

#include <iostream>
#include <map>
#include <utility>

namespace sustainml {
namespace common {

    constexpr int INVALID_ID = -1;

    //!Node Names
    constexpr const char* TASK_ENCODER_NODE = "TASK_ENCODER_NODE";
    constexpr const char* ML_MODEL_NODE = "ML_MODEL_NODE";
    constexpr const char* HW_RESOURCES_NODE = "HW_RESOURCES_NODE";
    constexpr const char* CO2_TRACKER_NODE = "CO2_TRACKER_NODE";

    enum Topics
    {
        NODE_CONTROL,
        NODE_STATUS,
        USER_INPUT,
        ENCODED_TASK,
        ML_MODEL,
        HW_RESOURCE,
        CO2_FOOTPRINT,
        MAX
    };

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
                    {USER_INPUT, {"/sustainml/user_input", "UserInputImpl"}},
                    {ENCODED_TASK, {"/sustainml/task_encoder/output", "EncodedTaskImpl"}},
                    {ML_MODEL, {"/sustainml/ml_model_provider/output", "MLModelImpl"}},
                    {HW_RESOURCE, {"/sustainml/hw_resources/output", "HWResourceImpl"}},
                    {CO2_FOOTPRINT, {"/sustainml/carbon_tracker/output", "CO2FootprintImpl"}}
                };

                return topics;
            }

        protected:

            TopicCollection() = default;
    };


    enum QueueIds
    {
        USER_INPUT_QUEUE,
        ENCODED_TASK_QUEUE,
        ML_MODEL_QUEUE,
        HW_RESOURCE_QUEUE,
        CO2_FOOTPRINT_QUEUE,
    };

    inline int queue_name_to_id(const std::string& queue_name)
    {
        auto tp = TopicCollection::get();

        if (queue_name.find(tp[ENCODED_TASK].second) != std::string::npos)
        {
            return ENCODED_TASK_QUEUE;
        }
        else if (queue_name.find(tp[USER_INPUT].second) != std::string::npos)
        {
            return USER_INPUT_QUEUE;
        }
        else if (queue_name.find(tp[ML_MODEL].second) != std::string::npos)
        {
            return ML_MODEL_QUEUE;
        }
        else if (queue_name.find(tp[HW_RESOURCE].second) != std::string::npos)
        {
            return HW_RESOURCE_QUEUE;
        }
        else if (queue_name.find(tp[CO2_FOOTPRINT].second) != std::string::npos)
        {
            return CO2_FOOTPRINT_QUEUE;
        }
        else
        {
            EPROSIMA_LOG_ERROR(COMMON, "Could not find the requested queue type name " << queue_name);
        }

        return -1;
    }

    template<typename T>
    inline int sample_type_to_queue_id(T* sample)
    {
        if (typeid(sample) == typeid(types::EncodedTask*))
        {
            return ENCODED_TASK_QUEUE;
        }
        else if (typeid(sample) == typeid(types::UserInput*))
        {
            return USER_INPUT_QUEUE;
        }
        else if (typeid(sample) == typeid(types::MLModel*))
        {
            return ML_MODEL_QUEUE;
        }
        else if (typeid(sample) == typeid(types::HWResource*))
        {
            return HW_RESOURCE_QUEUE;
        }
        else if (typeid(sample) == typeid(types::CO2Footprint*))
        {
            return CO2_FOOTPRINT_QUEUE;
        }

        return -1;
    }

    template<typename T>
    inline bool pair_queue_id_with_sample_type(
            std::vector<std::pair<int,void*>> input_samples,
            T* &sample,
            const size_t &expected_samples,
            size_t &n_samples_retrieved)
    {
        bool ret = false;

        for (std::pair<int,void*> &input : input_samples)
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

    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if<I == sizeof...(Tp), void>::type
    pair_queue_id_with_sample_type(
            const std::vector<std::pair<int,void*>> &input_samples,
            std::tuple<Tp...>& t, const size_t &expected_samples,
            size_t &n_samples_retrieved)
    { }

    template<std::size_t I = 0, typename... Tp>
    inline typename std::enable_if<I < sizeof...(Tp), void>::type
    pair_queue_id_with_sample_type(const std::vector<std::pair<int,void*>> &input_samples,
            std::tuple<Tp...>& t_args,
            const size_t &expected_samples,
            size_t &n_samples_retrieved)
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
