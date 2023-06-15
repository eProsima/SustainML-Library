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


#ifndef SUSTAINMLCPP_CORE_NODE_HPP
#define SUSTAINMLCPP_CORE_NODE_HPP

#include <config/macros.hpp>

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

    /**
    * @brief This abstract class is the principal class of the project.
    * Handles the DDS comunications, aggregates the dispatcher and provides
    * the main methods for interacting with the user.
    *
    * This class is meant to be inherited by the different
    * SustainML Nodes.
    */
    class Node
    {

        friend class Dispatcher;

    public:

        SUSTAINML_CPP_DLL_API Node();

        SUSTAINML_CPP_DLL_API ~Node();

        /**
        * @brief Called by the user to run the run.
        */
        SUSTAINML_CPP_DLL_API void spin();

        /**
        * @brief Stops the execution of the node.
        */
        SUSTAINML_CPP_DLL_API static void terminate();

    protected:

        /**
        * @brief Getter for the dispatcher
        *
        * @return A weak pointer to the Dispatcher object
        */
        std::weak_ptr<Dispatcher> get_dispatcher()
        {
            return dispatcher_;
        }

        /**
        * @brief Starts a new subscription (DataReader) in the
        * given topic.
        *
        * @param topic The topic name
        * @param listener Listener object inheriting from DataReaderListener
        */
        void initialize_subscription(
            const char* topic,
            eprosima::fastdds::dds::DataReaderListener* listener);

        /**
        * @brief Starts a new publication (DataWriter) in the
        * given topic.
        *
        * @param topic The topic name
        */
        void initialize_publication(
            const char* topic);

        /**
        * @brief Invokes the user callback with the provided inputs.
        *
        * @param inputs A vector containing the required samples. All the samples
        * must correspond to the same task_id.
        */
        virtual void publish_to_user(const std::vector<void*> inputs) = 0;

        std::shared_ptr<Dispatcher> dispatcher_;

        std::unique_ptr<eprosima::fastdds::dds::DomainParticipant> participant_;

        std::unique_ptr<eprosima::fastdds::dds::Publisher> publisher_;

        std::unique_ptr<eprosima::fastdds::dds::Subscriber> subscriber_;

        std::vector<std::unique_ptr<eprosima::fastdds::dds::Topic>> topics_;

        std::vector<std::unique_ptr<eprosima::fastdds::dds::DataWriter>> writers_;

        std::vector<std::unique_ptr<eprosima::fastdds::dds::DataReader>> readers_;

        std::mutex spin_mtx_;

        static std::condition_variable spin_cv_;

        static std::atomic<bool> terminate_;

    };

}

#endif // SUSTAINMLCPP_CORE_NODE_HPP
