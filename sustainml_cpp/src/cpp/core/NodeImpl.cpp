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
#include <core/Dispatcher.hpp>
#include <core/NodeImpl.hpp>
#include <core/Options.hpp>
#include <types/typesImplPubSubTypes.hpp>
#include <types/typesImplTypeObjectSupport.hpp>

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
    , req_res_listener_(*new RequestReplyListener())
    , control_listener_(this)
    , stop_task_callback_(nullptr)
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
    , req_res_listener_(*new RequestReplyListener())
    , control_listener_(this)
    , stop_task_callback_(nullptr)
{
    if (!init(name, opts))
    {
        EPROSIMA_LOG_ERROR(NODE, "Initialization Failed with the provided Options");
    }
}

NodeImpl::NodeImpl(
        Node* node,
        const std::string& name,
        RequestReplyListener& req_res_listener)
    : node_(node)
    , dispatcher_(new Dispatcher(node_))
    , participant_(nullptr)
    , publisher_(nullptr)
    , subscriber_(nullptr)
    , req_res_listener_(req_res_listener)
    , control_listener_(this)
    , stop_task_callback_(nullptr)
{
    if (!init(name))
    {
        EPROSIMA_LOG_ERROR(NODE, "Initialization Failed");
    }
}

NodeImpl::NodeImpl(
        Node* node,
        const std::string& name,
        const Options& opts,
        RequestReplyListener& req_res_listener)
    : node_(node)
    , dispatcher_(new Dispatcher(node_))
    , participant_(nullptr)
    , publisher_(nullptr)
    , subscriber_(nullptr)
    , req_res_listener_(req_res_listener)
    , control_listener_(this)
    , stop_task_callback_(nullptr)
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

    delete req_res_;

    dispatcher_->stop();
}

bool NodeImpl::init(
        const std::string& name,
        const Options& opts)
{
    dispatcher_->start();

    auto dpf = DomainParticipantFactory::get_instance();

    //! Initialize entities
    DomainParticipantQos pqos = opts.pqos;
    pqos.name(name);

    //! Set sustainML app ID participant properties
    pqos.properties().properties().emplace_back("fastdds.application.id", "SUSTAINML", true);
    pqos.properties().properties().emplace_back("fastdds.application.metadata", "", true);

    uint32_t domain = common::parse_sustainml_env(opts.domain);
    participant_ = dpf->create_participant(domain, pqos);

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

    req_res_ = new RequestReplier([this, name](void* input)
                    {
                        RequestTypeImpl* in = static_cast<RequestTypeImpl*>(input);

                        if (in->node_id() == static_cast<int32_t>(common::get_node_id_from_name(name)))
                        {
                            types::ResponseType res;
                            types::RequestType req;
                            req = *in;
                            req_res_listener_.on_configuration_request(req, res);
                            req_res_->write_res(res.get_impl());
                        }
                    }, "sustainml/response", "sustainml/request", participant_, publisher_, subscriber_, &req_data_);

    //! Register Common Types

    std::vector<eprosima::fastdds::dds::TypeSupport> sustainml_types;
    sustainml_types.reserve(common::Topics::MAX);

    sustainml_types.push_back(static_cast<TypeSupport>(new NodeStatusImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeControlImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new AppRequirementsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new CO2FootprintImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWConstraintsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWResourceImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelMetadataImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new UserInputImplPubSubType()));

    for (auto&& type : sustainml_types)
    {
        participant_->register_type(type);
    }

    //! Initialize common topics
    std::string name_no_node = name.substr(0, name.size() - std::string("_NODE").size());
    std::string filter_expr   = "target_node = %0";
std::vector<std::string> filter_params{ std::string("'") + name_no_node + "'" };
    std::cout << "Node name without _NODE: " << name_no_node << std::endl;
    initialize_subscription_content_filter(common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
            common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(),
            filter_expr.c_str(),
            filter_params,
            &control_listener_,
            opts);

    initialize_publication(common::TopicCollection::get()[common::Topics::NODE_STATUS].first.c_str(),
            common::TopicCollection::get()[common::Topics::NODE_STATUS].second.c_str(),
            opts);

    //! Initialize node
    node_status_.node_name(name);
    node_status_.node_status(Status::NODE_INITIALIZING);

    publish_node_status();

    return true;
}

void NodeImpl::spin()
{
    EPROSIMA_LOG_INFO(NODE, "Spinning Node... ");

    node_status_.node_status(Status::NODE_IDLE);
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

bool NodeImpl::initialize_subscription_content_filter(
        const char* topic_name,
        const char* type_name,
        const char* filter_expression,
        const std::vector<std::string>& filter_parameters,
        eprosima::fastdds::dds::DataReaderListener* listener,
        const Options& opts)
{
    Topic* topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);

    if (topic == nullptr)
    {
        return false;
    }

    for (const auto& parameter : filter_parameters)
    {
        std::cout << "Content Filter Parameter: " << parameter << std::endl;
    }
    std::string filter_name = std::string(topic_name) + "_cf";
    ContentFilteredTopic* filter_topic = participant_->create_contentfilteredtopic(filter_name.c_str(), topic, filter_expression, filter_parameters);

    if (filter_topic == nullptr)
    {
        return false;
    }

    DataReader* reader = subscriber_->create_datareader(filter_topic, opts.rqos, listener);

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

void NodeImpl::stop_current_task(const TaskIdImpl& task_id)
{
    if (stop_task_callback_)
    {
        EPROSIMA_LOG_INFO(NODE, "Stopping task " << task_id.problem_id() << "." << task_id.iteration_id());
        stop_task_callback_(task_id);
    }
    else
    {
        EPROSIMA_LOG_WARNING(NODE, "No stop task callback registered for task " << task_id.problem_id() << "." << task_id.iteration_id());
    }
}

void NodeImpl::register_stop_task_callback(std::function<void(const TaskIdImpl&)> callback)
{
    stop_task_callback_ = callback;
    EPROSIMA_LOG_INFO(NODE, "Stop task callback registered successfully");
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
    std::cout << "[CONTROL] NodeControl subscription matched" << std::endl;

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
    std::cout << "[CONTROL] NodeControl data available" << std::endl;
    EPROSIMA_LOG_INFO(NODE, "NodeStatus has a new status ");
    // NodeControl data
    NodeControlImpl control;
    eprosima::fastdds::dds::SampleInfo info;

    while (reader->take_next_sample(&control, &info) == RETCODE_OK)
    {
        if (! info.valid_data)
            continue;

        std::string cmd_task_str;
        switch (control.cmd_task())
        {
            case CmdTask::NO_CMD_TASK:
            cmd_task_str = "NO_CMD_TASK";
            break;
            case CmdTask::STOP_TASK:
            cmd_task_str = "STOP_TASK";
            // Call stop_task() here if needed
            node_->stop_current_task(control.task_id());
            break;
            case CmdTask::RESET_TASK:
            cmd_task_str = "RESET_TASK";
            break;
            case CmdTask::PREEMPT_TASK:
            cmd_task_str = "PREEMPT_TASK";
            break;
            case CmdTask::TERMINATE_TASK:
            cmd_task_str = "TERMINATE_TASK";
            break;
            default:
            cmd_task_str = "UNKNOWN";
            break;
        }

        std::cout << "[CONTROL] recibido cmd=" << cmd_task_str
              << " de " << control.source_node()
              << " para task=" << control.task_id().problem_id() << "."
              << control.task_id().iteration_id()
              << std::endl;
    }
}

} // namespace core
} // namespace sustainml
