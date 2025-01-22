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
 * @file RequestReplier.cpp
 */

#include <core/RequestReplier.hpp>

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace core {

RequestReplier::RequestReplier(
    std::function<void(void*)> callback,
    const char* topicw,
    const char* topicr)
    : callback_(callback)
    , topicw_(topicw)
    , topicr_(topicr)
    , participant_(nullptr)
    , typeRes_(new ResponseTypeImplPubSubType())
    , typeReq_(new RequestTypeImplPubSubType())
    , publisher_(nullptr)
    , subscriber_(nullptr)
    , listener_(this)
{
    auto dpf = DomainParticipantFactory::get_instance();

    DomainParticipantQos pqos;

    // Create a DomainParticipant
    participant_ = dpf->create_participant(0, pqos);

    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);

    // Register the type
    typeRes_.register_type(participant_);
    typeReq_.register_type(participant_);

    // Create a Topics
    if(std::string(topicw_) == "sustainml/request")
    {
    topicR_= participant_->create_topic(topicr_, typeRes_.get_type_name(), TOPIC_QOS_DEFAULT);
    topicW_= participant_->create_topic(topicw_, typeReq_.get_type_name(), TOPIC_QOS_DEFAULT);
    }
    else
    {
    topicR_= participant_->create_topic(topicr_, typeReq_.get_type_name(), TOPIC_QOS_DEFAULT);
    topicW_= participant_->create_topic(topicw_, typeRes_.get_type_name(), TOPIC_QOS_DEFAULT);
    }

    // Configure DataReader QoS
    DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
    rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    rqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    rqos.history().kind = KEEP_LAST_HISTORY_QOS;
    rqos.history().depth = 1;

    // Create a DataReader
    reader_ = subscriber_->create_datareader(topicR_, rqos, &listener_);

    // Configure DataWriter QoS
    DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
    wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    wqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    wqos.history().kind = KEEP_LAST_HISTORY_QOS;
    wqos.history().depth = 1;

    // Create a DataWriter
    writer_ = publisher_->create_datawriter(topicW_, wqos);
}

RequestReplier::~RequestReplier()
{

}

void RequestReplier::write_res(
    ResponseTypeImpl* res)
{
    writer_->write(res);
}

void RequestReplier::write_req(
    RequestTypeImpl* req)
{
    writer_->write(req);
}

RequestReplier::RequestReplyControlListener::RequestReplyControlListener(
    RequestReplier* node)
    : node_(node)
{

}

RequestReplier::RequestReplyControlListener::~RequestReplyControlListener()
{

}

void RequestReplier::RequestReplyControlListener::on_data_available(
    eprosima::fastdds::dds::DataReader* reader)
{
    // Create a data and SampleInfo instance
    SampleInfo info;
    void* data;

    if (reader->get_topicdescription()->get_type_name() == node_->typeReq_.get_type_name())
    {
        data = &req_;

    }
    else if (reader->get_topicdescription()->get_type_name() == node_->typeRes_.get_type_name())
    {
        data = &res_;
    }
    else
    {
        return;
    }

    // Keep taking data until there is nothing to take
    while (reader->take_next_sample(data, &info) == RETCODE_OK)
    {
        if (info.valid_data)
        {
            std::cout << "Received new data value for topic "
                << reader->get_topicdescription()->get_name()
                << " with type "
                << reader->get_topicdescription()->get_type_name()
                << std::endl;
            node_->callback_(data);
        }
        else
        {
            std::cout << "Remote writer for topic "
                << reader->get_topicdescription()->get_name()
                << " is dead" << std::endl;
        }
    }
}

void RequestReplier::RequestReplyControlListener::on_subscription_matched(
    eprosima::fastdds::dds::DataReader* reader,
    const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
{

    // New remote DataWriter discovered
    if (status.current_count_change == 1)
    {
    matched_ = status.current_count;
    std::cout << "Subscriber matched." << std::endl;
    }
    // New remote DataWriter undiscovered
    else if (status.current_count_change == -1)
    {
    matched_ = status.current_count;
    std::cout << "Subscriber unmatched." << std::endl;
    }
    // Non-valid option
    else
    {
    std::cout << status.current_count_change
          << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
    }
}

} // namespace core
} // namespace sustainml
