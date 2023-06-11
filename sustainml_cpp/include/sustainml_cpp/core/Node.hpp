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
 * @file Node.hpp
 */


#ifndef SUSTAINML_CPP__NODE_HPP
#define SUSTAINML_CPP__NODE_HPP

#include <utility>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>

namespace sustainml {

    class Dispatcher;

    class Node
    {

        friend class Dispatcher;

    public:

        Node();

        ~Node();

        void spin();

        static void terminate();

    protected:

        std::weak_ptr<Dispatcher> get_dispatcher()
        {
            return dispatcher_;
        }

        void initialize_subscription(
            const char* topic,
            eprosima::fastdds::dds::DataReaderListener* listener);

        void initialize_publication(
            const char* topic);

        virtual void publish_to_user(const std::vector<void*> inputs) = 0;

        std::shared_ptr<Dispatcher> dispatcher_;

        eprosima::fastdds::dds::DomainParticipant* participant_;

        eprosima::fastdds::dds::Publisher* publisher_;

        eprosima::fastdds::dds::Subscriber* subscriber_;

        std::vector<eprosima::fastdds::dds::Topic*> topics_;

        std::vector<eprosima::fastdds::dds::DataWriter*> writers_;

        std::vector<eprosima::fastdds::dds::DataReader*> readers_;

        std::mutex spin_mtx_;

        static std::condition_variable spin_cv_;

        static std::atomic<bool> terminate_;

    };

}

#endif // SUSTAINML_CPP__NODE_HPP
