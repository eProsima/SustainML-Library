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
 * @file Options.hpp
 */

#ifndef SUSTAINMLCPP_CORE_OPTIONS_HPP
#define SUSTAINMLCPP_CORE_OPTIONS_HPP

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>

namespace sustainml {
namespace core {

/**
 * @brief Options structure to define Node entities QoS (Participant, Subscriber, Publisher, DataWriter
 * and DataReader). Those QoS are set in initialize_publication and initialize_subscription methods.
 */
struct Options
{
    eprosima::fastdds::dds::DomainId_t domain{0};
    eprosima::fastdds::dds::DomainParticipantQos pqos;
    eprosima::fastdds::dds::SubscriberQos subqos;
    eprosima::fastdds::dds::PublisherQos pubqos;
    eprosima::fastdds::dds::DataReaderQos rqos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
    eprosima::fastdds::dds::DataWriterQos wqos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;
    std::size_t sample_pool_size{50};
};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_OPTIONS_HPP
