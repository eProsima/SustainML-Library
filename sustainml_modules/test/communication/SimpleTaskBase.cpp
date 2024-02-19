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

#include "SimpleTaskBase.hpp"

#include "types/typesImplTypeObject.h"

SimpleTaskBase::~SimpleTaskBase()
{
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->
            delete_participant(participant_);
}

bool SimpleTaskBase::init(
        uint32_t&,
        std::string& topic_name,
        eprosima::fastdds::dds::TypeSupport& ts)
{
    eprosima::fastdds::dds::DomainParticipantQos pqos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;

    auto factory =
            eprosima::fastdds::dds::DomainParticipantFactory::get_instance();

    participant_ = factory->create_participant(0, pqos);

    if (participant_ == nullptr)
    {
        return false;
    }

    type_ = ts;

    //REGISTER THE TYPE
    type_.register_type(participant_);

    registertypesImplTypes();

    //CREATE THE TOPIC
    eprosima::fastdds::dds::TopicQos tqos =
            eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;

    topic_ = participant_->create_topic(
        topic_name,
        type_.get_type_name(),
        tqos);

    if (topic_ == nullptr)
    {
        return false;
    }

    return true;
}
