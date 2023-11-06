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
 * @file QueuedNodeListener.hpp
 */

#ifndef SUSTAINMLCPP_CORE_QUEUEDNODELISTENER_HPP
#define SUSTAINMLCPP_CORE_QUEUEDNODELISTENER_HPP

#include <sustainml_cpp/core/NodeListener.hpp>
#include <sustainml_cpp/core/SamplesQueue.hpp>
#include <sustainml_cpp/interfaces/QueueQueryable.hpp>

#include <core/SamplesQueue.cpp>
#include <core/NodeListener.cpp>

namespace sustainml {
namespace core {

    class Node;

    /**
    * @brief Aggregates the core components for receving and storing
    * a sample of a particular type in a queue.
    * It can also be queried for a particular sample.
    */
    template <typename T>
    class QueuedNodeListener : public NodeListener<T>,
                               public SamplesQueue<T>,
                               public interfaces::QueueQueryable<T>
    {

    public:

        QueuedNodeListener(
            Node* node,
            const Options& opts = Options())
            : NodeListener<T>(node, this)
            , SamplesQueue<T>(node, opts)
        {

        }

        virtual ~QueuedNodeListener() = default;

        /**
        * @brief Retrieves the queue of the particular type.
        *
        * @return Pointer to the queue.
        */
        SamplesQueue<T>* get_queue() override
        {
            return SamplesQueue<T>::get_queue();
        }

    };

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_QUEUEDNODELISTENER_HPP
