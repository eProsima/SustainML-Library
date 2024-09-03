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
 * @file TaskInjector.hpp
 *
 */

#ifndef _TEST_BLACKBOX_TASKINJECTOR_HPP_
#define _TEST_BLACKBOX_TASKINJECTOR_HPP_

#include <condition_variable>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>

template <typename T>
class TaskInjector
{

    struct TaskInjectorListener : public eprosima::fastdds::dds::DataWriterListener
    {
        TaskInjectorListener(
                TaskInjector* task_injector)
            : task_inj_(task_injector)
        {

        }

        virtual void on_publication_matched(
                eprosima::fastdds::dds::DataWriter* writer,
                const eprosima::fastdds::dds::PublicationMatchedStatus& info)
        {
            if (info.current_count_change == 1)
            {
                task_inj_->matched_.fetch_add(1);
                std::cout << "TaskInjector matched" << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                task_inj_->matched_.fetch_sub(1);
                std::cout << "TaskInjector unmatched" << std::endl;
            }

            task_inj_->cv_.notify_all();
        }

        TaskInjector* task_inj_;
    };

public:

    TaskInjector(
            const std::string& topic_name
            )
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , datawriter_(nullptr)
        , type_(new T())
        , listener_(this)
        , matched_(0)
    {
        eprosima::fastdds::dds::DomainParticipantQos pqos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;
        pqos.name("TaskInjector_Participant");
        auto factory =
                eprosima::fastdds::dds::DomainParticipantFactory::get_instance();

        participant_ = factory->create_participant(0, pqos);

        if (participant_ == nullptr)
        {
            return;
        }

        //REGISTER THE TYPE
        type_.register_type(participant_);

        //CREATE THE PUBLISHER
        eprosima::fastdds::dds::PublisherQos pubqos =
                eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT;

        publisher_ = participant_->create_publisher(
            pubqos,
            nullptr);

        if (publisher_ == nullptr)
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

        // CREATE THE WRITER
        eprosima::fastdds::dds::DataWriterQos wqos =
                eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;

        wqos.resource_limits().max_instances = 500;
        wqos.resource_limits().max_samples_per_instance = 1;

        datawriter_ = publisher_->create_datawriter(
            topic_,
            wqos,
            &listener_);

        if (datawriter_ == nullptr)
        {
            return;
        }
    }

    ~TaskInjector()
    {
        if (datawriter_ != nullptr)
        {
            publisher_->delete_datawriter(datawriter_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
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

        std::cout << "TrajInjector is waiting discovery..." << std::endl;

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

        std::cout << "Writer discovery finished..." << std::endl;

        return matched_ == expected_matches;
    }

    void inject(
            std::list<typename T::type>& msgs,
            const int& ms_period = 50)
    {
        auto it = msgs.begin();

        while (it != msgs.end())
        {
            if (eprosima::fastdds::dds::RETCODE_OK ==
                    datawriter_->write((void*)&(*it)))
            {
                std::cout << "Injecting data with task id {" << it->task_id().problem_id() << "," <<
                    it->task_id().iteration_id() << "}" << std::endl;
                it = msgs.erase(it);
                std::this_thread::sleep_for(std::chrono::milliseconds(ms_period));
            }
            else
            {
                break;
            }
        }
    }

private:

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Publisher* publisher_;

    eprosima::fastdds::dds::Topic* topic_;

    eprosima::fastdds::dds::DataWriter* datawriter_;

    eprosima::fastdds::dds::TypeSupport type_;

    TaskInjectorListener listener_;

    std::atomic<size_t> matched_;

    std::condition_variable cv_;

    std::mutex mtx_;

};

#endif // _TEST_BLACKBOX_TASKINJECTOR_HPP_
