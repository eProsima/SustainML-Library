#include <sustainml_cpp/core/Options.hpp>

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>

#ifndef SUSTAINML_CORE_OPTIONS_CPP
#define SUSTAINML_CORE_OPTIONS_CPP

namespace sustainml {
namespace core {

Options::Options() : sample_pool_size(50), domain(0)
{
    pqos = new eprosima::fastdds::dds::DomainParticipantQos();
    subqos = new eprosima::fastdds::dds::SubscriberQos();
    pubqos = new eprosima::fastdds::dds::PublisherQos();
    rqos = new eprosima::fastdds::dds::DataReaderQos();
    wqos = new eprosima::fastdds::dds::DataWriterQos();
}

} // namespace core
} // namespace sustainml

#endif //SUSTAINML_CORE_OPTIONS_CPP
