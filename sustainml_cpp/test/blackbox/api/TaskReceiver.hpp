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
 * @file TaskReceiver.hpp
 *
 */

#ifndef _TEST_BLACKBOX_TASKRECEIVER_HPP_
#define _TEST_BLACKBOX_TASKRECEIVER_HPP_

#include <condition_variable>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>

template <typename T>
class TaskReceiver
{

    struct TaskReceiverListener : public eprosima::fastdds::dds::DataReaderListener
    {
        TaskReceiverListener(
                TaskReceiver* task_receiver)
            : task_inj_(task_receiver)
        {

        }

        virtual void on_data_available(
                eprosima::fastdds::dds::DataReader* reader)
        {
            typename T::type data;
            eprosima::fastdds::dds::SampleInfo info;

            if (eprosima::fastdds::dds::RETCODE_OK ==
                    reader->take_next_sample((void*)&data, &info))
            {
                std::cout << "TaskReceiver receive data for service from node id " << data.node_id() <<
                    " ,with transaction " << data.transaction_id() << "." << std::endl;

                {
                    std::unique_lock<std::mutex> lock(task_inj_->mtx_);
                    task_inj_->data_ = data;
                }
                task_inj_->cv_.notify_one();
            }
        }

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
        {
            if (status.current_count_change == 1)
            {
                task_inj_->matched_.fetch_add(1);
                std::cout << "TaskReceiver matched" << std::endl;
            }
            else if (status.current_count_change == -1)
            {
                task_inj_->matched_.fetch_sub(1);
            }
        }

        TaskReceiver* task_inj_;
    };

public:

    TaskReceiver(
            const std::string& topic_name
            )
        : participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , datareader_(nullptr)
        , type_(new T())
        , listener_(this)
        , matched_(0)
    {
        eprosima::fastdds::dds::DomainParticipantQos pqos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;
        pqos.name("TaskReceiver_Participant");
        auto factory =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance();

        participant_ = factory->create_participant(0, pqos);

        if (participant_ == nullptr)
        {
            return;
        }

        //REGISTER THE TYPE
        type_.register_type(participant_);

        //CREATE THE SUBSCRIBER
        eprosima::fastdds::dds::SubscriberQos pubqos =
                eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;

        subscriber_ = participant_->create_subscriber(
            pubqos,
            nullptr);

        if (subscriber_ == nullptr)
        {
            return;
        }

        //CREATE THE TOPIC
        eprosima::fastdds::dds::TopicQos tqos =
                eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;

        topic_ = participant_->create_topic(
            topic_name,
            type_.get_type_name(),
            tqos);

        if (topic_ == nullptr)
        {
            return;
        }

        // CREATE THE READER
        eprosima::fastdds::dds::DataReaderQos rqos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
        rqos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
        rqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
        rqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
        rqos.history().depth = 1;

        rqos.resource_limits().max_instances = 500;
        rqos.resource_limits().max_samples_per_instance = 1;

        datareader_ = subscriber_->create_datareader(
            topic_,
            rqos,
            &listener_);

        if (datareader_ == nullptr)
        {
            return;
        }
    }

    ~TaskReceiver()
    {
        if (datareader_ != nullptr)
        {
            subscriber_->delete_datareader(datareader_);
        }
        if (subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->
                delete_participant(participant_);
    }

    bool wait_discovery(
            const size_t& expected_matches,
            std::chrono::seconds timeout = std::chrono::seconds::zero())
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if (timeout == std::chrono::seconds::zero())
        {
            cv_.wait(lock, [&]()
                    {
                        return matched_ == expected_matches;
                    });
        }
        else
        {
            cv_.wait_for(lock, timeout, [&]()
                    {
                        return matched_ == expected_matches;
                    });
        }

        std::cout << "Reader discovery finished..." << std::endl;

        return matched_ == expected_matches;
    }

    typename T::type wait_for_data(
            std::chrono::milliseconds timeout)
    {
        auto start = std::chrono::steady_clock::now();

        std::unique_lock<std::mutex> lock(mtx_);
        if (!cv_.wait_for(lock, timeout, [this]() -> bool
                {
                    return data_ != typename T::type();
                }))
        {
            std::cerr << "Error: Timeout while waiting for data." << std::endl;
            return typename T::type();
        }

        typename T::type response = data_;
        data_ = typename T::type();
        return response;
    }

private:

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Subscriber* subscriber_;

    eprosima::fastdds::dds::Topic* topic_;

    eprosima::fastdds::dds::DataReader* datareader_;

    eprosima::fastdds::dds::TypeSupport type_;

    TaskReceiverListener listener_;

    std::atomic<size_t> matched_;

    std::condition_variable cv_;

    std::mutex mtx_;

    typename T::type data_;

};

#endif // _TEST_BLACKBOX_TASKRECEIVER_HPP_
