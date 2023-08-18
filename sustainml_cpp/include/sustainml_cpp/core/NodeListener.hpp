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
 * @file NodeListener.hpp
 */


#ifndef SUSTAINMLCPP_CORE_NODELISTENER_HPP
#define SUSTAINMLCPP_CORE_NODELISTENER_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/interfaces/QueueQueryable.hpp>

#include <fastdds/dds/subscriber/DataReaderListener.hpp>

namespace sustainml {
namespace core {

    class Node;

    /**
    * @brief Aggregates a sample queue in which to place new incoming
    * samples and notifies the Dispatcher.
    * Implements the DataReaderListener callbacks.
    */
    template <typename T>
    class NodeListener : public eprosima::fastdds::dds::DataReaderListener
    {

    public:

        NodeListener(
            Node* node,
            interfaces::QueueQueryable<T>* qq);

        virtual ~NodeListener();

        /**
        * @brief Stops the listener.
        *
        */
        void stop();

        /**
        * @brief Callback executed when a new sample is available on the DataReader.
        *
        * @param reader The DataReader having new available samples.
        */
        void on_data_available(
            eprosima::fastdds::dds::DataReader* reader);

        /**
        * @brief Callback executed when a DataReader matching status change.
        *
        * @param reader The DataReader.
        * @param status The status of the subscription.
        */
        void on_subscription_matched(
            eprosima::fastdds::dds::DataReader* reader,
            const eprosima::fastdds::dds::SubscriptionMatchedStatus & status);

    private:

        Node* node_;
        interfaces::QueueQueryable<T>* queue_queryable_;
        std::atomic<bool> stop_;

    };

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_NODELISTENER_HPP
