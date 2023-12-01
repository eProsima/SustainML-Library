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

#ifndef _TEST_COMMUNICATION_SIMPLETASKBASE_HPP_
#define _TEST_COMMUNICATION_SIMPLETASKBASE_HPP_

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/topic/qos/TopicQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

class SimpleTaskBase
{
public:

    SimpleTaskBase() = default;
    virtual ~SimpleTaskBase();

    virtual bool init(
            uint32_t& samples,
            std::string& topic_name,
            eprosima::fastdds::dds::TypeSupport& ts);

    virtual bool run() = 0;

protected:

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Topic* topic_;

    eprosima::fastdds::dds::TypeSupport type_;

};

#endif // _TEST_COMMUNICATION_SIMPLETASKBASE_HPP_