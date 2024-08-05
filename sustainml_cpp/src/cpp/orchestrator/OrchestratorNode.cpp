// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file OrchestratorNode.cpp
 */

#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>

#include "ModuleNodeProxyFactory.hpp"
#include "TaskDB.ipp"

#include <common/Common.hpp>
#include <orchestrator/TaskManager.hpp>
#include <types/typesImplPubSubTypes.h>
#include <types/typesImplTypeObject.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace orchestrator {

OrchestratorNode::OrchestratorParticipantListener::OrchestratorParticipantListener(
        OrchestratorNode* orchestrator)
    : orchestrator_(orchestrator)
{

}

void OrchestratorNode::OrchestratorParticipantListener::on_participant_discovery(
        eprosima::fastdds::dds::DomainParticipant* participant,
        eprosima::fastrtps::rtps::ParticipantDiscoveryInfo&& info)
{
    eprosima::fastrtps::string_255 participant_name = info.info.m_participantName;
    EPROSIMA_LOG_INFO(ORCHESTRATOR, "Orchestrator discovered a new Participant with name " << participant_name);

    // Synchronise with Orchestrator initialization
    if (!orchestrator_->initialized_.load())
    {
        std::unique_lock<std::mutex> lock(orchestrator_->get_mutex());
        orchestrator_->initialization_cv_.wait(lock, [&]()
                {
                    return orchestrator_->initialized_.load();
                });
    }

    // create the proxy for this node
    NodeID node_id = common::get_node_id_from_name(participant_name);

    std::lock_guard<std::mutex> lock(orchestrator_->proxies_mtx_);
    if (info.status == info.DISCOVERED_PARTICIPANT &&
            orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)] == nullptr)
    {
        EPROSIMA_LOG_INFO(ORCHESTRATOR, "Creating node proxy for " << participant_name << " node");
        ModuleNodeProxyFactory::make_node_proxy(
            node_id,
            orchestrator_,
            orchestrator_->task_db_,
            orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]);
    }
    else if ((info.status == info.DROPPED_PARTICIPANT || info.status == info.REMOVED_PARTICIPANT) &&
            orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)] != nullptr)
    {
        EPROSIMA_LOG_INFO(ORCHESTRATOR, "Setting inactive " << participant_name << " node");
        types::NodeStatus status = orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]->get_status();
        status.node_status(NODE_INACTIVE);
        orchestrator_->node_proxies_[static_cast<uint32_t>(node_id)]->set_status(status);
        orchestrator_->handler_->on_node_status_change(node_id, status);
    }
}

OrchestratorNode::OrchestratorNode(
        std::shared_ptr<OrchestratorNodeHandle> handle,
        uint32_t domain)
    : domain_(domain)
    , handler_(handle)
    , node_proxies_({
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        }),
    task_db_(new TaskDB_t()),
    task_man_(new TaskManager()),
    participant_listener_(new OrchestratorParticipantListener(this))
{
    if (!init())
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Orchestrator initialization Failed");
    }
}

OrchestratorNode::~OrchestratorNode()
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (size_t i = 0; i < (size_t)NodeID::MAX; i++)
    {
        std::lock_guard<std::mutex> lock(proxies_mtx_);
        if (node_proxies_[i] != nullptr)
        {
            delete node_proxies_[i];
        }
    }

    if (sub_ != nullptr)
    {
        sub_->delete_contained_entities();
    }

    if (pub_ != nullptr)
    {
        pub_->delete_contained_entities();
    }

    if (participant_ != nullptr)
    {
        participant_->delete_contained_entities();
    }

    DomainParticipantFactory::get_instance()->delete_participant(participant_);

    delete task_man_;
}

void OrchestratorNode::print_db()
{
    std::cout << *task_db_ << std::endl;
}

bool OrchestratorNode::init()
{
    auto dpf = DomainParticipantFactory::get_instance();

    DomainParticipantQos dpqos = PARTICIPANT_QOS_DEFAULT;
    dpqos.name("ORCHESTRATOR_NODE");

    //! Set sustainML app ID participant properties
    dpqos.properties().properties().emplace_back("fastdds.application.id", "SUSTAINML", true);
    dpqos.properties().properties().emplace_back("fastdds.application.metadata", "", true);

    //! Initialize entities
    participant_ = dpf->create_participant(domain_,
                    dpqos,
                    participant_listener_.get(),
                    StatusMask::all() >> StatusMask::data_on_readers());

    if (participant_ == nullptr)
    {
        return false;
    }

    //! Register Common Types
    std::vector<eprosima::fastdds::dds::TypeSupport> sustainml_types;
    sustainml_types.reserve(common::Topics::MAX);

    sustainml_types.push_back(static_cast<TypeSupport>(new AppRequirementsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new CO2FootprintImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWConstraintsImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new HWResourceImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new MLModelMetadataImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeControlImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new NodeStatusImplPubSubType()));
    sustainml_types.push_back(static_cast<TypeSupport>(new UserInputImplPubSubType()));

    for (auto&& type : sustainml_types)
    {
        participant_->register_type(type);
    }

    // Register type objects
    registertypesImplTypes();

    status_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::NODE_STATUS].first.c_str(),
        common::TopicCollection::get()[common::Topics::NODE_STATUS].second.c_str(), TOPIC_QOS_DEFAULT);

    control_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::NODE_CONTROL].first.c_str(),
        common::TopicCollection::get()[common::Topics::NODE_CONTROL].second.c_str(), TOPIC_QOS_DEFAULT);

    user_input_topic_ = participant_->create_topic(
        common::TopicCollection::get()[common::Topics::USER_INPUT].first.c_str(),
        common::TopicCollection::get()[common::Topics::USER_INPUT].second.c_str(), TOPIC_QOS_DEFAULT);

    if (status_topic_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the status topic");
        return false;
    }

    pub_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);

    if (pub_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the publisher");
        return false;
    }

    DataWriterQos dwqos = DATAWRITER_QOS_DEFAULT;
    dwqos.resource_limits().max_instances = 500;
    dwqos.resource_limits().max_samples_per_instance = 1;
    dwqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;

    control_writer_ = pub_->create_datawriter(control_topic_, dwqos);

    if (control_writer_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the control writer");
        return false;
    }

    user_input_writer_ = pub_->create_datawriter(user_input_topic_, dwqos);

    if (user_input_topic_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the user input writer");
        return false;
    }

    sub_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    if (sub_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Error creating the subscriber");
        return false;
    }

    initialized_.store(true);
    initialization_cv_.notify_one();
    return true;
}

std::pair<types::TaskId, types::UserInput*> OrchestratorNode::prepare_new_task()
{
    std::pair<types::TaskId, types::UserInput*> output;
    auto task_id = task_man_->create_new_task_id();
    {
        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        task_db_->prepare_new_entry_nts(task_id, false);
        task_db_->get_task_data_nts(task_id, output.second);
    }
    output.first = task_id;
    return output;
}

std::pair<types::TaskId, types::UserInput*> OrchestratorNode::prepare_new_iteration(
        const types::TaskId& old_task_id)

{
    std::pair<types::TaskId, types::UserInput*> output;
    types::TaskId new_task_id(old_task_id);
    new_task_id.iteration_id(new_task_id.iteration_id() + 1);
    {
        std::lock_guard<std::mutex> lock(task_db_->get_mutex());
        task_db_->prepare_new_entry_nts(new_task_id, true);
        // Copy the UserInput from the previous iteration
        // it also updates the iteration_id in the data
        task_db_->copy_data_nts(old_task_id, new_task_id, {NodeID::ID_ORCHESTRATOR});
        task_db_->get_task_data_nts(new_task_id, output.second);
    }
    output.first = new_task_id;
    return output;
}

bool OrchestratorNode::start_task(
        const types::TaskId& task_id,
        types::UserInput* ui)
{
    user_input_writer_->write(ui->get_impl());
    publish_baselines(task_id);
    return true;
}

bool OrchestratorNode::start_iteration(
        const types::TaskId& task_id,
        types::UserInput* ui)
{
    user_input_writer_->write(ui->get_impl());
    publish_baselines(task_id);
    return true;
}

void OrchestratorNode::publish_baselines(
        const types::TaskId& task_id)
{
    //publish in the iteration topics
    for (size_t i = 0; i < (size_t)NodeID::MAX; i++)
    {
        std::lock_guard<std::mutex> lock(proxies_mtx_);
        if (node_proxies_[i] != nullptr &&
                node_proxies_[i]->publishes_baseline())
        {
            node_proxies_[i]->publish_data_for_iteration(task_id);
        }
    }
}

RetCode_t OrchestratorNode::get_task_data(
        const types::TaskId& task_id,
        const NodeID& node_id,
        void*& data)
{
    RetCode_t ret = RetCode_t::RETCODE_NO_DATA;

    std::lock_guard<std::mutex> lock(task_db_->get_mutex());

    switch (node_id)
    {
        case NodeID::ID_ML_MODEL_METADATA:
        {
            MapFromNodeIDToType_t<NodeID::ID_ML_MODEL_METADATA>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_ML_MODEL:
        {
            MapFromNodeIDToType_t<NodeID::ID_ML_MODEL>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_HW_RESOURCES:
        {
            MapFromNodeIDToType_t<NodeID::ID_HW_RESOURCES>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_CARBON_FOOTPRINT:
        {
            MapFromNodeIDToType_t<NodeID::ID_CARBON_FOOTPRINT>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_HW_CONSTRAINTS:
        {
            MapFromNodeIDToType_t<NodeID::ID_HW_CONSTRAINTS>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_APP_REQUIREMENTS:
        {
            MapFromNodeIDToType_t<NodeID::ID_APP_REQUIREMENTS>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
            break;
        }
        case NodeID::ID_ORCHESTRATOR:
        {
            MapFromNodeIDToType_t<NodeID::ID_ORCHESTRATOR>::type* typed_data = nullptr;
            if (task_db_->get_task_data_nts(task_id, typed_data))
            {
                data = typed_data;
                //! Check if the task_id is the same as the one requested
                //! meaning that the data has already been received
                if (typed_data->task_id() == task_id)
                {
                    ret = RetCode_t::RETCODE_OK;
                }
            }
        }
        default:
        {
            EPROSIMA_LOG_ERROR(ORCHESTRATOR, "Requested Data from non-existing node ID");
            break;
        }
    }

    return ret;
}

RetCode_t OrchestratorNode::get_node_status(
        const NodeID& node_id,
        const types::NodeStatus*& status)
{
    RetCode_t ret = RetCode_t::RETCODE_NO_DATA;

    if ((int)node_id >= 0 && node_id < NodeID::MAX)
    {
        std::lock_guard<std::mutex> lock(proxies_mtx_);
        status = &node_proxies_[(int)node_id]->get_status();
        ret = RetCode_t::RETCODE_OK;
    }

    return ret;
}

void OrchestratorNode::send_control_command(
        const types::NodeControl& cmd)
{
    control_writer_->write(cmd.get_impl());
}

} // namespace orchestrator
} // namespace orchestrator

