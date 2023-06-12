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
 * @file Node.cpp
 */

#include <sustainml_cpp/core/Node.hpp>

namespace sustainml {

    std::atomic<bool> Node::terminate_(false);
    std::condition_variable Node::spin_cv_;

    void Node::spin()
    {

    }

    void Node::initialize_subscription(
        const char* topic,
        eprosima::fastdds::dds::DataReaderListener* listener)
    {

    }

    void Node::initialize_publication(
        const char* topic)
    {

    }

    void Node::terminate()
    {

    }

    void Node::publish_to_user(const std::vector<void*> inputs)
    {

    }

} // namespace sustainml
