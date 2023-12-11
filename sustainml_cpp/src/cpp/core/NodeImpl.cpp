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

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <common/Common.hpp>
#include <core/NodeImpl.hpp>
#include <core/Dispatcher.hpp>
#include <types/typesImplPubSubTypes.h>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace core {

std::atomic<bool> NodeImpl::terminate_(false);
std::condition_variable NodeImpl::spin_cv_;

NodeImpl::NodeImpl(
        Node* node,
        const std::string& name)
    : node_(node)
    , dispatcher_(new Dispatcher(node_))
    , participant_(nullptr)
    , publisher_(nullptr)
    , subscriber_(nullptr)
    , control_listener_(this)
{
    if (!init(name))
    {
        EPROSIMA_LOG_ERROR(NODE, "Initialization Failed");
    }
}

NodeImpl::NodeImpl(
        Node* node,
        const std::string& name,
        const Options& opts)
    : node_(node)
    , dispatcher_(new Dispatcher(node_))
    , participant_(nullptr)
    , publisher_(nullptr)
    , subscriber_(nullptr)
    , control_listener_(this)
{
    if (!init(name, opts))
    {
        EPROSIMA_LOG_ERROR(NODE, "Initialization Failed with the provided Options");
    }
}

NodeImpl::~NodeImpl()
{
    EPROSIMA_LOG_INFO(NODE, "Destroying Node");

    if (nullptr != participant_)
    {
        participant_->delete_contained_entities();
        auto dpf = DomainParticipantFactory::get_instance();
        dpf->delete_participant(participant_);
    }

    dispatcher_->stop();
}

bool NodeImpl::init(
        const std::string& name,
        const Options& opts)
{
    dispatcher_->start();

    auto dpf = DomainParticipantFactory::get_instance();

    //! Initialize entities

    participant_ = dpf->create_participant(opts.domain, opts.pqos);

    if (participant_ == nullptr)
    {
        return false;
    }

    subscriber_ = participant_->create_subscriber(opts.subqos);

    if (subscriber_ == nullptr)
    {
        return false;
    }

    publisher_ = participant_->create_publisher(opts.pubqos);

    if (publisher_ == nullptr)
    {
        return false;
    }

    //! Register Common Types

    std::vector<eprosima::fastdds::dds::TypeSupport> sustainml_types;
    sustainml_types.reserve(common::Topics::MAX);

    sustainml_types.push_back(static_cast<TypeSupport>(new NodeStatusImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeControlImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new UserInputImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new EncodedTaskImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWResourceImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new CO2FootprintImplPubSubType()));

    for (auto&& type : sustainml_types)
    {
        participant_->register_type(type);
    }

    //! Initialize common topics
    initialize_subscription(common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
            common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(),
            &control_listener_, opts);

    initialize_publication(common::TopicCollection::get()[common::Topics::NODE_STATUS].first.c_str(),
            common::TopicCollection::get()[common::Topics::NODE_STATUS].second.c_str(),
            opts);

    //! Initialize node
    node_status_.node_name(name);
    node_status_.node_status(::NODE_INITIALIZING);

    publish_node_status();

    return true;
}

void NodeImpl::spin()
{
    EPROSIMA_LOG_INFO(NODE, "Spinning Node... ");

    node_status_.node_status(::NODE_IDLE);
    publish_node_status();

    std::unique_lock<std::mutex> lock(spin_mtx_);
    spin_cv_.wait(lock, [&]
            {
                return terminate_.load();
            });
}

bool NodeImpl::initialize_subscription(
        const char* topic_name,
        const char* type_name,
        eprosima::fastdds::dds::DataReaderListener* listener,
        const Options& opts)
{
    Topic* topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);

    if (topic == nullptr)
    {
        return false;
    }

    DataReader* reader = subscriber_->create_datareader(topic, opts.rqos, listener);

    if (reader == nullptr)
    {
        return false;
    }

    topics_.emplace_back(topic);
    readers_.emplace_back(reader);

    return true;
}

bool NodeImpl::initialize_publication(
        const char* topic_name,
        const char* type_name,
        const Options& opts)
{
    Topic* topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);

    if (topic == nullptr)
    {
        return false;
    }

    DataWriter* writer = publisher_->create_datawriter(topic, opts.wqos);

    if (writer == nullptr)
    {
        return false;
    }

    topics_.emplace_back(topic);
    writers_.emplace_back(writer);

    return true;
}

void NodeImpl::publish_node_status()
{
    if (!writers_.empty())
    {
        writers_[STATUS_WRITER_IDX]->write(&node_status_);
    }
}

void NodeImpl::terminate()
{
    terminate_.store(true);
    spin_cv_.notify_all();
}

NodeImpl::NodeControlListener::NodeControlListener(
        NodeImpl* node)
    : node_(node)
{

}

NodeImpl::NodeControlListener::~NodeControlListener()
{

}

void NodeImpl::NodeControlListener::on_subscription_matched(
        eprosima::fastdds::dds::DataReader* reader,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
{

    EPROSIMA_LOG_INFO(NODE, "NodeControl Reader Suscription status");

    // New remote DataWriter discovered
    if (status.current_count_change == 1)
    {
        matched_ = status.current_count;
        EPROSIMA_LOG_INFO(NODE, "Subscriber matched.");
    }
    // New remote DataWriter undiscovered
    else if (status.current_count_change == -1)
    {
        matched_ = status.current_count;
        EPROSIMA_LOG_INFO(NODE, "Subscriber unmatched.");
    }
    // Non-valid option
    else
    {
        EPROSIMA_LOG_INFO(NODE, status.current_count_change
                << " is not a valid value for SubscriptionMatchedStatus current count change");
    }
}

void NodeImpl::NodeControlListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    EPROSIMA_LOG_INFO(NODE, "NodeStatus has a new status ");
}

} // namespace core
} // namespace sustainml
