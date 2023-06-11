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

#ifndef SUSTAINML_CPP__QUEUEDNODELISTENER_HPP
#define SUSTAINML_CPP__QUEUEDNODELISTENER_HPP

#include <sustainml_cpp/core/NodeListener.hpp>
#include <sustainml_cpp/core/SamplesQueue.hpp>
#include <sustainml_cpp/interfaces/QueueQueryable.hpp>

namespace sustainml {

    class Node;

    template <typename T>
    class QueuedNodeListener : public NodeListener<T>,
                               public SamplesQueue<T>,
                               public QueueQueryable<T>
    {

    public:

        QueuedNodeListener(
            Node* node);

        ~QueuedNodeListener();

        SamplesQueue<T>* get_queue() override
        {
            return SamplesQueue<T>::get_queue();
        }

    };

}

#endif // SUSTAINML_CPP__QUEUEDNODELISTENER_HPP
