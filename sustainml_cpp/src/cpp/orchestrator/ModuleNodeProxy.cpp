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
 * @file ModuleNodeProxy.cpp
 */


#include <orchestrator/ModuleNodeProxy.hpp>
#include <orchestrator/TaskManager.hpp>

#include "TaskDB.ipp"

#include <common/Common.hpp>

namespace sustainml {
namespace orchestrator {

ModuleNodeProxy::ModuleNodeProxyListener::ModuleNodeProxyListener(
        ModuleNodeProxy* proxy)
    : proxy_parent_(proxy)
{

}

void ModuleNodeProxy::ModuleNodeProxyListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    eprosima::fastdds::dds::SampleInfo info;

    void* tmp_untyped_impl_data = proxy_parent_->get_tmp_impl_untyped_data();
    if (reader->take_next_sample(tmp_untyped_impl_data, &info) == eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK)
    {
        if (info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE)
        {
            EPROSIMA_LOG_INFO(MODULE_PROXY, "notify_new_node_ouput " << proxy_parent_->name_);
            proxy_parent_->notify_new_node_ouput();
        }
    }
}

void ModuleNodeProxy::ModuleNodeProxyListener::on_subscription_matched(
        eprosima::fastdds::dds::DataReader* reader,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
{

}

ModuleNodeProxy::ModuleNodeProxyStatusListener::ModuleNodeProxyStatusListener(
        ModuleNodeProxy* proxy)
    : proxy_parent_(proxy)
{

}

void ModuleNodeProxy::ModuleNodeProxyStatusListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    SampleInfo info;
    // Take next sample from DataReader's history
    if (ReturnCode_t::RETCODE_OK == reader->take_next_sample(proxy_parent_->status_.get_impl(), &info))
    {
        // Some samples only update the instance state. Only if it is a valid sample (with data)
        if (ALIVE_INSTANCE_STATE == info.instance_state)
        {
            // Print structure data
            EPROSIMA_LOG_INFO(MODULE_PROXY,
                    "New Status " << proxy_parent_->status_.node_name() << " " << proxy_parent_->status_.node_status() <<
                    " RECEIVED");
            proxy_parent_->notify_status_change();
        }
    }
}

void ModuleNodeProxy::ModuleNodeProxyStatusListener::on_subscription_matched(
        eprosima::fastdds::dds::DataReader*,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
{

}

ModuleNodeProxy::ModuleNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<TaskDB_t> task_db,
        const char* name,
        bool need_to_publish_baseline)
    : name_(name)
    , node_id_(common::get_node_id_from_name(name_))
    , publish_baseline_(need_to_publish_baseline)
    , orchestrator_(orchestrator)
    , task_db_(task_db)
    , baseline_topic_(nullptr)
    , baseline_writer_(nullptr)
    , listener_(this)
    , status_listener_(this)
{
    if (orchestrator_->participant_ == nullptr ||
            orchestrator_->sub_ == nullptr ||
            orchestrator_->status_topic_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_NODE_PROXY, "Error construction");
        return;
    }

    status_.node_name(name);

    node_output_topic_ = orchestrator_->participant_->create_topic(
        common::TopicCollection::get()[common::get_topic_from_name(name, false)].first.c_str(),
        common::TopicCollection::get()[common::get_topic_from_name(name, false)].second.c_str(), TOPIC_QOS_DEFAULT);

    if (node_output_topic_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_NODE_PROXY, "Error creating the status topic in " << name);
        return;
    }

    DataReaderQos drqos;
    drqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;
    drqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    drqos.resource_limits().max_instances = 500;
    drqos.resource_limits().max_samples_per_instance = 1;
    drqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    drqos.history().depth = 1;

    node_output_datareader_ = orchestrator_->sub_->create_datareader(node_output_topic_, drqos, &listener_);

    std::string expression("node_name like %0");
    std::vector<std::string> parameters;
    parameters.push_back(std::string("'") + name + "'");

    filtered_status_topic_ = orchestrator_->participant_->create_contentfilteredtopic(
        (common::TopicCollection::get()[common::Topics::NODE_STATUS].first + "_" + name).c_str(),
        orchestrator_->status_topic_,
        expression,
        parameters);

    status_datareader_ = orchestrator_->sub_->create_datareader(filtered_status_topic_, drqos, &status_listener_);

    if (status_datareader_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_NODE_PROXY, "Error creating the status datareader in " << name);
        return;
    }

    std::string baseline_topic_name;
    if (publish_baseline_)
    {
        baseline_topic_name = (common::TopicCollection::get()[common::get_topic_from_name(name_, true)].first);
        baseline_topic_ = orchestrator_->participant_->create_topic(
            baseline_topic_name,
            common::TopicCollection::get()[common::get_topic_from_name(name_, true)].second.c_str(), TOPIC_QOS_DEFAULT);

        if (baseline_topic_ == nullptr)
        {
            EPROSIMA_LOG_ERROR(ORCHESTRATOR_NODE_PROXY, "Error creating the iteration topic in " << name);
            return;
        }

        baseline_writer_ = orchestrator_->pub_->create_datawriter(
            baseline_topic_,
            DATAWRITER_QOS_DEFAULT,
            nullptr);

        if (baseline_writer_ == nullptr)
        {
            EPROSIMA_LOG_ERROR(ORCHESTRATOR_NODE_PROXY, "Error creating the iteration writer in " << name);
            return;
        }
    }
}

ModuleNodeProxy::~ModuleNodeProxy()
{
    if (status_datareader_)
    {
        status_datareader_->set_listener(nullptr);
    }

    if (node_output_datareader_)
    {
        node_output_datareader_->set_listener(nullptr);
    }
}

void ModuleNodeProxy::notify_status_change()
{
    std::lock_guard<std::mutex> lock(orchestrator_->get_mutex());
    std::shared_ptr<OrchestratorNodeHandle> handler_ptr = orchestrator_->get_handler().lock();
    if (handler_ptr != nullptr)
    {
        handler_ptr->on_node_status_change(node_id_, status_);
    }
}

void ModuleNodeProxy::notify_new_node_ouput()
{
    store_data_in_db();
    void* untyped_data = get_tmp_untyped_data();
    std::lock_guard<std::mutex> lock(orchestrator_->get_mutex());
    std::shared_ptr<OrchestratorNodeHandle> handler_ptr = orchestrator_->get_handler().lock();
    if (handler_ptr != nullptr)
    {
        handler_ptr->on_new_node_output(node_id_, untyped_data);
    }
}

void ModuleNodeProxy::reset_and_prepare_task_id_nts(
        const types::TaskId& task_id)
{
    task_db_->prepare_new_entry_nts(task_id, false);
    orchestrator_->task_man_->update_task_id(task_id);
}

void ModuleNodeProxy::set_status(
        const types::NodeStatus& status)
{
    status_ = status;
}

const types::NodeStatus& ModuleNodeProxy::get_status()
{
    return status_;
}

AppRequirementsNodeProxy::AppRequirementsNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::APP_REQUIREMENTS_NODE, need_to_publish_baseline)
{

}

void AppRequirementsNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::AppRequirements* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void AppRequirementsNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

CarbonFootprintNodeProxy::CarbonFootprintNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::CARBON_FOOTPRINT_NODE, need_to_publish_baseline)
{

}

void CarbonFootprintNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::CO2Footprint* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void CarbonFootprintNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

HardwareConstraintsNodeProxy::HardwareConstraintsNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::HW_CONSTRAINTS_NODE, need_to_publish_baseline)
{

}

void HardwareConstraintsNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::HWConstraints* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void HardwareConstraintsNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

HardwareResourcesNodeProxy::HardwareResourcesNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::HW_RESOURCES_NODE, need_to_publish_baseline)
{

}

void HardwareResourcesNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::HWResource* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void HardwareResourcesNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

MLModelMetadataNodeProxy::MLModelMetadataNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::ML_MODEL_METADATA_NODE, need_to_publish_baseline)
{

}

void MLModelMetadataNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::MLModelMetadata* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void MLModelMetadataNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

MLModelProviderNodeProxy::MLModelProviderNodeProxy(
        OrchestratorNode* orchestrator,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
        bool need_to_publish_baseline)
    : ModuleNodeProxy(orchestrator, task_db, common::ML_MODEL_NODE, need_to_publish_baseline)
{

}

void MLModelProviderNodeProxy::publish_data_for_iteration(
        const types::TaskId& task_id)
{
    types::MLModel* iter_data = nullptr;
    ModuleNodeProxy::publish_data_for_iteration_(task_id, iter_data);
}

void MLModelProviderNodeProxy::store_data_in_db()
{
    std::lock_guard<std::mutex> lock(task_db_->get_mutex());
    if (!task_db_->entry_exists_nts(tmp_data_.task_id()))
    {
        reset_and_prepare_task_id_nts(tmp_data_.task_id());
    }
    task_db_->insert_task_data_nts(tmp_data_.task_id(), tmp_data_);
}

} // namespace orchestrator
} // namespace sustainml

#include "ModuleNodeProxy.ipp"