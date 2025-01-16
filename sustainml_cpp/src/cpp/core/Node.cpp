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

#include <core/NodeImpl.hpp>
#include <core/Options.hpp>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace core {

Node::Node(
        const std::string& name)
{
    impl_ = new NodeImpl(this, name);
}

Node::Node(
        const std::string& name,
        const Options& opts)
{
    impl_ = new NodeImpl(this, name, opts);
}

Node::Node(
        const std::string& name,
        RequestReplyListener& req_res_listener)
{
    impl_ = new NodeImpl(this, name, req_res_listener);
}

Node::Node(
        const std::string& name,
        const Options& opts,
        RequestReplyListener& req_res_listener)
{
    impl_ = new NodeImpl(this, name, opts, req_res_listener);
}

Node::~Node()
{
    delete impl_;
}

void Node::spin()
{
    impl_->spin();
}

bool Node::initialize_subscription(
        const char* topic_name,
        const char* type_name,
        eprosima::fastdds::dds::DataReaderListener* listener,
        const Options& opts)
{
    return impl_->initialize_subscription(topic_name, type_name, listener, opts);
}

bool Node::initialize_publication(
        const char* topic_name,
        const char* type_name,
        const Options& opts)
{
    return impl_->initialize_publication(topic_name, type_name, opts);
}

void Node::publish_node_status()
{
    impl_->publish_node_status();
}

void Node::terminate()
{
    NodeImpl::terminate();
}

const std::string& Node::name()
{
    return impl_->node_status_.node_name();
}

const Status& Node::status()
{
    return impl_->node_status_.node_status();
}

void Node::status(
        const Status& status)
{
    impl_->node_status_.node_status(status);
}

std::weak_ptr<Dispatcher> Node::get_dispatcher()
{
    return impl_->get_dispatcher();
}

const std::vector<eprosima::fastdds::dds::DataWriter*>& Node::writers()
{
    return impl_->writers_;
}

} // namespace core
} // namespace sustainml
