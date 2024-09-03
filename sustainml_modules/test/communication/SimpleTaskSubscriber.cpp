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

#include "SimpleTaskSubscriber.hpp"

#include <csignal>

std::atomic<bool> SimpleTaskSubscriber::stop_(false);
std::condition_variable SimpleTaskSubscriber::cv_;

SimpleTaskSubscriber::SimpleTaskSubscriber()
    : expected_samples_(0)
    , subscriber_(nullptr)
    , datareader_(nullptr)
    , listener_(this)
{

}

SimpleTaskSubscriber::~SimpleTaskSubscriber()
{
    if (datareader_ != nullptr)
    {
        subscriber_->delete_datareader(datareader_);
    }
    if (subscriber_ != nullptr)
    {
        participant_->delete_subscriber(subscriber_);
    }
}

bool SimpleTaskSubscriber::init(
        uint32_t& samples,
        std::string& topic_name,
        eprosima::fastdds::dds::TypeSupport& ts)
{
    SimpleTaskBase::init(samples, topic_name, ts);

    expected_samples_ = samples;

    //CREATE THE SUBSCRIBER
    eprosima::fastdds::dds::SubscriberQos subqos =
            eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;

    subscriber_ = participant_->create_subscriber(
        subqos,
        nullptr);

    if (subscriber_ == nullptr)
    {
        return false;
    }

    // CREATE THE READER
    eprosima::fastdds::dds::DataReaderQos rqos =
            eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;

    //! Accept a high number of instances
    rqos.resource_limits().max_instances = 500;
    rqos.resource_limits().max_samples_per_instance = 1;

    //! Set reliable QoS
    rqos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;

    //! Set history QoS
    rqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    rqos.history().depth = 1;

    datareader_ = subscriber_->create_datareader(
        topic_,
        rqos,
        &listener_);

    if (datareader_ == nullptr)
    {
        return false;
    }

    return true;
}

void SimpleTaskSubscriber::SimpleTaskSubscriberListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    eprosima::fastdds::dds::SampleInfo info;
    void* data = task_sub_->type_->create_data();
    while ((reader->take_next_sample(data, &info) == eprosima::fastdds::dds::RETCODE_OK)
        && !task_sub_->is_stopped())
    {
        if (info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE)
        {
            samples_++;
            std::cout << "Sample of type " << task_sub_->type_.get_type_name() << " RECEIVED" << std::endl;
            if (samples_ >= task_sub_->expected_samples_)
            {
                stop();
            }
        }
    }
}

bool SimpleTaskSubscriber::run()
{
    stop_ = false;
    if (expected_samples_ > 0)
    {
        std::cout << "Subscriber running until " << expected_samples_ <<
            " samples have been received. Please press CTRL+C to stop the Subscriber at any time." << std::endl;
    }
    else
    {
        std::cout << "Subscriber running. Please press CTRL+C to stop the Subscriber." << std::endl;
    }
    signal(SIGINT, [](int signum)
            {
                std::cout << "SIGINT received, stopping Subscriber execution." << std::endl;
                static_cast<void>(signum); SimpleTaskSubscriber::stop();
            });

    std::unique_lock<std::mutex> lck(mtx_);
    cv_.wait(lck, [&]
            {
                return is_stopped();
            });

    return listener_.samples_ >= expected_samples_;
}
