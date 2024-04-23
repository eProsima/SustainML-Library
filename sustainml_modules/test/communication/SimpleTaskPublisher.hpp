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

#ifndef _TEST_COMMUNICATION_SIMPLETASKPUBLISHER_HPP_
#define _TEST_COMMUNICATION_SIMPLETASKPUBLISHER_HPP_

#include "SimpleTaskBase.hpp"

#include <condition_variable>
#include <thread>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>

class SimpleTaskPublisher : public SimpleTaskBase
{
    struct SimpleTaskPublisherListener : public eprosima::fastdds::dds::DataWriterListener
    {
        SimpleTaskPublisherListener(
                SimpleTaskPublisher* task_pub)
            : task_pub_(task_pub)
        {

        }

        virtual void on_publication_matched(
                eprosima::fastdds::dds::DataWriter* writer,
                const eprosima::fastdds::dds::PublicationMatchedStatus& info)
        {
            if (info.current_count_change == 1)
            {
                std::cout << "SimpleTaskPublisher matched" << std::endl;
                task_pub_->matched_ = true;
                task_pub_->cv_.notify_all();
            }
            else if (info.current_count_change == -1)
            {
                std::cout << "SimpleTaskPublisher unmatched" << std::endl;
            }

        }

        SimpleTaskPublisher* task_pub_;
    };

public:

    SimpleTaskPublisher();
    virtual ~SimpleTaskPublisher();

    virtual bool init(
            uint32_t& samples,
            std::string& topic_name,
            eprosima::fastdds::dds::TypeSupport& ts) override;

    virtual bool run() override;

private:

    void wait_discovery();

    uint32_t samples_;

    eprosima::fastdds::dds::Publisher* publisher_;

    eprosima::fastdds::dds::DataWriter* datawriter_;

    std::mutex mtx_;

    std::condition_variable cv_;

    bool matched_;

    SimpleTaskPublisherListener listener_;

};

#endif // _TEST_COMMUNICATION_SIMPLETASKPUBLISHER_HPP_