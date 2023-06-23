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

#include <sustainml_cpp/core/Dispatcher.hpp>
#include <sustainml_cpp/core/Node.hpp>

#include <common/Common.hpp>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace core {

    std::atomic<bool> Node::terminate_(false);
    std::condition_variable Node::spin_cv_;

    Node::Node(const std::string &name) :
        dispatcher_(new Dispatcher(this)),
        control_listener_(this),
        participant_(nullptr),
        subscriber_(nullptr),
        publisher_(nullptr)
    {
        if (!init(name))
        {
            EPROSIMA_LOG_ERROR(NODE, "Initialization Failed");
        }
    }

    Node::Node(const std::string &name,
               const Options& opts) :
        dispatcher_(new Dispatcher(this)),
        control_listener_(this),
        participant_(nullptr),
        subscriber_(nullptr),
        publisher_(nullptr)
    {
        if (!init(name, opts))
        {
            EPROSIMA_LOG_ERROR(NODE, "Initialization Failed with the provided Options");
        }
    }

    Node::~Node()
    {
        EPROSIMA_LOG_INFO(NODE, "Destroying Node");
        dispatcher_->stop();
    }

    bool Node::init(const std::string& name, const Options& opts)
    {
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

        sustainml_types.push_back(static_cast<TypeSupport>(new NodeStatusPubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new NodeControlPubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new UserInputPubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new EncodedTaskPubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new MLModelPubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new HWResourcePubSubType()));
        sustainml_types.push_back(static_cast<TypeSupport>(new CO2FootprintPubSubType()));

        for (auto &&type : sustainml_types)
        {
            participant_->register_type(type);
        }

        //! Initialize common topics
        initialize_subscription(common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
                                common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(),
                                &control_listener_);

        initialize_publication(common::TopicCollection::get()[common::Topics::NODE_STATUS].first.c_str(),
                                common::TopicCollection::get()[common::Topics::NODE_STATUS].second.c_str());

        //! Initialize node
        node_status_.node_name(name);
        node_status_.node_status(NODE_INITIALIZING);

        publish_node_status();

        return true;
    }

    void Node::spin()
    {
        EPROSIMA_LOG_INFO(NODE, "Spinning Node... ");
        dispatcher_->start();

        node_status_.node_status(NODE_IDLE);
        publish_node_status();

        std::unique_lock<std::mutex> lock(spin_mtx_);
        spin_cv_.wait(lock, [&]{ return terminate_.load();});
    }

    bool Node::initialize_subscription(
        const char* topic_name,
        const char* type_name,
        eprosima::fastdds::dds::DataReaderListener* listener)
    {
        Topic* topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);

        if (topic == nullptr)
        {
            return false;
        }

        DataReader* reader = subscriber_->create_datareader(topic, DATAREADER_QOS_DEFAULT, listener);

        if (reader == nullptr)
        {
            return false;
        }

        topics_.emplace_back(topic);
        readers_.emplace_back(reader);

        return true;
    }

    bool Node::initialize_publication(
        const char* topic_name,
        const char* type_name)
    {
        Topic* topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);

        if (topic == nullptr)
        {
            return false;
        }

        DataWriter* writer = publisher_->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);

        if (writer == nullptr)
        {
            return false;
        }

        topics_.emplace_back(topic);
        writers_.emplace_back(writer);

        return true;
    }

    void Node::publish_node_status()
    {
        if (!writers_.empty())
        {
            writers_[0]->write(&node_status_);
        }
    }

    void Node::terminate()
    {
        terminate_.store(true);
        spin_cv_.notify_all();
    }

    Node::NodeControlListener::NodeControlListener(Node *node)
        : node_(node)
    {

    }

    Node::NodeControlListener::~NodeControlListener()
    {

    }

    void Node::NodeControlListener::on_subscription_matched(
        eprosima::fastdds::dds::DataReader* reader,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus & status)
    {

        std::cout << "NodeControl Reader Suscription status ";

        // New remote DataWriter discovered
        if (status.current_count_change == 1)
        {
            matched_ = status.current_count;
            std::cout << "Subscriber matched." << std::endl;
        }
        // New remote DataWriter undiscovered
        else if (status.current_count_change == -1)
        {
            matched_ = status.current_count;
            std::cout << "Subscriber unmatched." << std::endl;
        }
        // Non-valid option
        else
        {
            std::cout << status.current_count_change
                    << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
        }
    }

    void Node::NodeControlListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
    {
        std::cout << "NodeStatus has a new status " << std::endl;
    }

} // namespace core
} // namespace sustainml
