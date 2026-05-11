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

#include <types/SustainMLServiceServer.hpp>
#include <types/SustainMLService.hpp>
#include <fastdds/dds/domain/qos/ReplierQos.hpp>
#include <fastdds/dds/rpc/exceptions.hpp>

#include <core/GenericServiceNodeImpl.hpp>

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
{
    if (!init(name, opts))
    {
        EPROSIMA_LOG_ERROR(NODE, "Initialization Failed with the provided Options");
    }
}

NodeImpl::~NodeImpl()
{
    shutting_down_.store(true, std::memory_order_release);

    EPROSIMA_LOG_INFO(NODE, "Destroying Node");

    if (rpc_server_)
    {
        rpc_server_->stop();

        if (rpc_server_thread_.joinable())
        {
            rpc_server_thread_.join();
        }
        rpc_server_.reset();
    }

    if (participant_)
    {
        participant_->delete_contained_entities();
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
        participant_ = nullptr;
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

    eprosima::fastdds::dds::ReplierQos rqos;

    try
    {
        if (name == common::APP_REQUIREMENTS_NODE)
        {
            std::shared_ptr<AppRequirementsServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::AppRequirementsServiceNodeImpl>(*this,
                            "APP_REQUIREMENTS");
            rpc_impl_ = impl;
            rpc_service_name_ = "AppRequirementsService";

            rpc_server_ = create_AppRequirementsServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else if (name == common::HW_CONSTRAINTS_NODE)
        {
            std::shared_ptr<HWConstraintsServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::HWConstraintsServiceNodeImpl>(*this, "HW_CONSTRAINTS");
            rpc_impl_ = impl;
            rpc_service_name_ = "HWConstraintsService";

            rpc_server_ = create_HWConstraintsServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else if (name == common::HW_RESOURCES_NODE)
        {
            std::shared_ptr<HWResourcesServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::HWResourcesServiceNodeImpl>(*this, "HW_RESOURCES");
            rpc_impl_ = impl;
            rpc_service_name_ = "HWResourcesService";

            rpc_server_ = create_HWResourcesServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else if (name == common::CARBON_FOOTPRINT_NODE)
        {
            std::shared_ptr<CarbonFootprintServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::CarbonFootprintServiceNodeImpl>(*this,
                            "CARBON_FOOTPRINT");
            rpc_impl_ = impl;
            rpc_service_name_ = "CarbonFootprintService";

            rpc_server_ = create_CarbonFootprintServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else if (name == common::ML_MODEL_METADATA_NODE)
        {
            std::shared_ptr<MLModelMetadataServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::MLModelMetadataServiceNodeImpl>(*this,
                            "ML_MODEL_METADATA");
            rpc_impl_ = impl;
            rpc_service_name_ = "MLModelMetadataService";

            rpc_server_ = create_MLModelMetadataServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else if (name == common::ML_MODEL_NODE)
        {
            std::shared_ptr<MLModelServiceServer_IServerImplementation> impl =
                    std::make_shared<sustainml::core::MLModelServiceNodeImpl>(*this, "ML_MODEL");
            rpc_impl_ = impl;
            rpc_service_name_ = "MLModelService";

            rpc_server_ = create_MLModelServiceServer(
                *participant_,
                rpc_service_name_.c_str(),
                rqos,
                1u,
                impl);
        }
        else
        {
            EPROSIMA_LOG_WARNING(NODE,
                    "No RPC service configured for node name '" << name << "'");
        }
    }
    catch (const eprosima::fastdds::dds::rpc::RpcException& e)
    {
        EPROSIMA_LOG_ERROR(NODE,
                "RpcException while creating RPC server for node '" << name
                                                                    << "': " << e.what());
    }
    catch (const std::exception& e)
    {
        EPROSIMA_LOG_ERROR(NODE,
                "std::exception while creating RPC server for node '" << name
                                                                      << "': " << e.what());
    }

    if (!rpc_server_)
    {
        EPROSIMA_LOG_WARNING(NODE,
                "No RPC server created for node '" << name
                                                   << "' (service_name='" << rpc_service_name_
                                                   << "'). Node will run without RPC.");
    }

    if (rpc_server_)
    {
        rpc_server_thread_ = std::thread([this]()
                        {
                            rpc_server_->run();
                        });
    }

    // std::cout << "[DEBUG NodeImpl] RPC server created on node '"
    //           << name
    //           << "' with service '" << rpc_service_name_ << "'" << std::endl;

    // Register Common Types

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
    initialize_subscription(common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
            common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(),
            &control_listener_, opts);

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
