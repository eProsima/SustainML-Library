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

#ifndef _TEST_COMMUNICATION_SIMPLETASKSUBSCRIBER_HPP_
#define _TEST_COMMUNICATION_SIMPLETASKSUBSCRIBER_HPP_

#include "SimpleTaskBase.hpp"

#include <condition_variable>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

class SimpleTaskSubscriber : public SimpleTaskBase
{

public:

    SimpleTaskSubscriber();
    virtual ~SimpleTaskSubscriber();

    virtual bool init(
            uint32_t& samples,
            std::string& topic_name,
            eprosima::fastdds::dds::TypeSupport& ts) override;

    virtual bool run() override;

    class SimpleTaskSubscriberListener : public eprosima::fastdds::dds::DataReaderListener
    {
    public:

        SimpleTaskSubscriberListener(
                SimpleTaskSubscriber* task_sub)
            : matched_(0)
            , samples_(0)
            , task_sub_(task_sub)
        {
        }

        ~SimpleTaskSubscriberListener() override
        {
        }

        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader) override;

        int matched_;
        uint32_t samples_;
        SimpleTaskSubscriber* task_sub_;
    };

    static void stop()
    {
        stop_ = true;
        cv_.notify_all();
    }

private:

    bool is_stopped()
    {
        return stop_.load();
    }

    uint32_t expected_samples_;

    eprosima::fastdds::dds::Subscriber* subscriber_;

    eprosima::fastdds::dds::DataReader* datareader_;

    SimpleTaskSubscriberListener listener_;

    std::mutex mtx_;

    static std::atomic<bool> stop_;

    static std::condition_variable cv_;

};

#endif // _TEST_COMMUNICATION_SIMPLETASKSUBSCRIBER_HPP_