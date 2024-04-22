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

#ifndef _TEST_COMMUNICATION_UTILS_TYPEFACTORY_HPP_
#define _TEST_COMMUNICATION_UTILS_TYPEFACTORY_HPP_

#include <bitset>

#include "EnumTypes.hpp"

#include "../types/typesImplPubSubTypes.h"

#include <fastdds/dds/topic/TypeSupport.hpp>

struct TypeFactory
{
    TypeFactory() = delete;
    ~TypeFactory() = delete;

    static eprosima::fastdds::dds::TypeSupport make_type(
            std::bitset<TopicType::MAX>& type_opts)
    {
        if (type_opts[USER_INPUT])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new UserInputImplPubSubType());
        }
        else if (type_opts[ML_MODEL_METADATA])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new MLModelMetadataImplPubSubType());
        }
        else if (type_opts[ML_MODEL])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new MLModelImplPubSubType());
        }
        else if (type_opts[HW_RESOURCES])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new HWResourceImplPubSubType());
        }
        else if (type_opts[CO2_FOOTPRINT])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new CO2FootprintImplPubSubType());
        }
        else if (type_opts[HW_CONSTRAINTS])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new HWConstraintsImplPubSubType());
        }
        else if (type_opts[APP_REQUIREMENTS])
        {
            return static_cast<eprosima::fastdds::dds::TypeSupport>(new AppRequirementsImplPubSubType());
        }


        return eprosima::fastdds::dds::TypeSupport();
    }

    static void set_data_task_id(
            eprosima::fastdds::dds::TypeSupport& ts,
            void* data,
            const size_t& task_id)
    {
        TaskIdImpl task_id_impl;
        task_id_impl.problem_id(task_id);
        task_id_impl.data_id(task_id);

        if (ts.get_type_name() == "UserInputImpl")
        {
            auto ui_data = static_cast<UserInputImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "MLModelMetadataImpl")
        {
            auto ui_data = static_cast<MLModelMetadataImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "MLModelImpl")
        {
            auto ui_data = static_cast<MLModelImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "HWResourceImpl")
        {
            auto ui_data = static_cast<HWResourceImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "CO2FootprintImpl")
        {
            auto ui_data = static_cast<CO2FootprintImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "HWResourceImpl")
        {
            auto ui_data = static_cast<HWResourceImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else if (ts.get_type_name() == "AppRequirementsImpl")
        {
            auto ui_data = static_cast<AppRequirementsImpl*>(data);
            ui_data->task_id(task_id_impl);
        }
        else
        {
            std::cout << "ERROR, Unknown DataType" << std::endl;
        }
    }

};

#endif // _TEST_COMMUNICATION_UTILS_TYPEFACTORY_HPP_

