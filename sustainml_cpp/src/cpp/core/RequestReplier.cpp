// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <common/Common.hpp>

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

namespace sustainml {
namespace core {

RequestReplier::RequestReplier(
        std::function<void(void*)> callback,
        const char* topicw,
        const char* topicr,
        DomainParticipant* participant,
        Publisher* publisher,
        Subscriber* subscriber,
        void* data)
    : callback_(callback)
    , topicr_(topicr)
    , topicw_(topicw)
    , data_(data)
    , participant_(participant)
    , publisher_(publisher)
    , subscriber_(subscriber)
    , typeRes_(new ResponseTypeImplPubSubType())
    , typeReq_(new RequestTypeImplPubSubType())
    , listener_(this)
{
    // Register the type
    typeRes_.register_type(participant_);
    typeReq_.register_type(participant_);

    // Create a Topics
    if (std::string(topicw_) == "sustainml/request")
    {
        topicR_ = participant_->create_topic(topicr_, typeRes_.get_type_name(), TOPIC_QOS_DEFAULT);
        topicW_ = participant_->create_topic(topicw_, typeReq_.get_type_name(), TOPIC_QOS_DEFAULT);
    }
    else
    {
        topicR_ = participant_->create_topic(topicr_, typeReq_.get_type_name(), TOPIC_QOS_DEFAULT);
        topicW_ = participant_->create_topic(topicw_, typeRes_.get_type_name(), TOPIC_QOS_DEFAULT);
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
    if (publisher_)
    {
        publisher_->delete_datawriter(writer_);
    }

    if (subscriber_)
    {
        subscriber_->delete_datareader(reader_);
    }
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

std::mutex& RequestReplier::get_mutex()
{
    return mtx_;
}

void RequestReplier::resume_taking_data()
{
    resume_taking_data_.store(true);
    cv_.notify_all();
}

void RequestReplier::wait_until(
        const std::function<bool()>& condition)
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, condition);
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

    ReturnCode_t ret = RETCODE_ERROR;
    do
    {
        // Ensure middleware can write on data_
        std::unique_lock<std::mutex> lock(node_->get_mutex());

        auto ret = reader->take_next_sample(node_->data_, &info);

        if (ret == RETCODE_OK && info.valid_data)
        {
            node_->resume_taking_data_.store(false);
            node_->callback_(node_->data_);
            node_->cv_.wait_for(lock, std::chrono::milliseconds(100), [this]
                    {
                        return node_->resume_taking_data_.load();
                    });
        }

    }
    while (ret == RETCODE_OK);
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
        EPROSIMA_LOG_ERROR(RequestReplier, status.current_count_change
                << " is not a valid value for SubscriptionMatchedStatus current count change");
    }
}

} // namespace core
} // namespace sustainml
