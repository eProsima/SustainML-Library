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


#ifndef SUSTAINML_CPP__NODELISTENER_HPP
#define SUSTAINML_CPP__NODELISTENER_HPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/interfaces/QueueQueryable.hpp>

#include <fastdds/dds/subscriber/DataReaderListener.hpp>

namespace sustainml {

    class Node;

    template <typename T>
    class NodeListener : public eprosima::fastdds::dds::DataReaderListener
    {

    public:

        NodeListener(
            Node* node,
            QueueQueryable<T>* qq);

        ~NodeListener();

        void on_data_available(
            eprosima::fastdds::dds::DataReader* reader)
        {
            //! TODO
        }

        void on_subscription_matched(
            eprosima::fastdds::dds::DataReader* reader,
            const eprosima::fastdds::dds::SubscriptionMatchedStatus & status)
        {
            //! TODO
        }

    private:

        T data_;
        Node* node_;
        QueueQueryable<T>* queue_queryable_;

    };

}

#endif // SUSTAINML_CPP__NODELISTENER_HPP
