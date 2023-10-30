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

#include <iostream>

namespace eprosima {
namespace fastdds {
namespace dds {

class DomainParticipantQos;
class SubscriberQos;
class PublisherQos;
class DataReaderQos;
class DataWriterQos;
} // namespace dds
} // namespace fastdds
} // namespace eprosima

namespace sustainml {
namespace core {
    /**
     * @brief Options structure to define Node entities QoS (Participant, Subscriber, Publisher, DataWriter
     * and DataReader). Those QoS are set in initialize_publication and initialize_subscription methods.
     */
    struct Options
    {
        Options();

        uint32_t domain;
        eprosima::fastdds::dds::DomainParticipantQos* pqos;
        eprosima::fastdds::dds::SubscriberQos* subqos;
        eprosima::fastdds::dds::PublisherQos* pubqos;
        eprosima::fastdds::dds::DataReaderQos* rqos;
        eprosima::fastdds::dds::DataWriterQos* wqos;
        std::size_t sample_pool_size;
    };
} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_OPTIONS_HPP
