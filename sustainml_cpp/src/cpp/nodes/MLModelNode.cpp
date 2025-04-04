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
 * @file MLModelNode.cpp
 */

#include <sustainml_cpp/nodes/MLModelNode.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>

#include <common/Common.hpp>
#include <core/Options.hpp>
#include <core/QueuedNodeListener.hpp>
#include <types/typesImpl.hpp>

using namespace types;

namespace sustainml {
namespace ml_model_module {

MLModelNode::MLModelNode(
        MLModelTaskListener& user_listener)
    : Node(common::ML_MODEL_NODE)
    , user_listener_(user_listener)
{
    sustainml::core::Options opts;
    opts.rqos.resource_limits().max_instances = 500;
    opts.rqos.resource_limits().max_samples_per_instance = 1;
    opts.rqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    opts.rqos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    opts.rqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    opts.rqos.history().depth = 1;

    opts.wqos.resource_limits().max_instances = 500;
    opts.wqos.resource_limits().max_samples_per_instance = 1;
    opts.wqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;

    init(opts);
}

MLModelNode::MLModelNode(
        MLModelTaskListener& user_listener,
        sustainml::core::RequestReplyListener& req_res_listener)
    : Node(common::ML_MODEL_NODE, req_res_listener)
    , user_listener_(user_listener)
{
    sustainml::core::Options opts;
    opts.rqos.resource_limits().max_instances = 500;
    opts.rqos.resource_limits().max_samples_per_instance = 1;
    opts.rqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    opts.rqos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    opts.rqos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    opts.rqos.history().depth = 1;

    opts.wqos.resource_limits().max_instances = 500;
    opts.wqos.resource_limits().max_samples_per_instance = 1;
    opts.wqos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;

    init(opts);
}

MLModelNode::MLModelNode(
        MLModelTaskListener& user_listener,
        sustainml::core::Options opts)
    : Node(common::ML_MODEL_NODE, opts)
    , user_listener_(user_listener)
{
    init(opts);
}

MLModelNode::MLModelNode(
        MLModelTaskListener& user_listener,
        sustainml::core::RequestReplyListener& req_res_listener,
        sustainml::core::Options opts)
    : Node(common::ML_MODEL_NODE, opts, req_res_listener)
    , user_listener_(user_listener)
{
    init(opts);
}

MLModelNode::~MLModelNode()
{

}

void MLModelNode::init (
        const sustainml::core::Options& opts)
{
    listener_model_metadata_queue_.reset(new core::QueuedNodeListener<MLModelMetadata>(this));
    listener_app_requirements_queue_.reset(new core::QueuedNodeListener<AppRequirements>(this));
    listener_hw_constraints_queue_.reset(new core::QueuedNodeListener<HWConstraints>(this));

    // Baselines
    listener_mlmodel_queue_.reset(new core::QueuedNodeListener<MLModel>(this));
    listener_hw_queue_.reset(new core::QueuedNodeListener<HWResource>(this));
    listener_carbon_footprint_queue_.reset(new core::QueuedNodeListener<CO2Footprint>(this));

    task_data_pool_.reset(new utils::SamplePool<types::NodeTaskOutputData<MLModel>>(opts));

    initialize_subscription(sustainml::common::TopicCollection::get()[common::ML_MODEL_METADATA].first.c_str(),
            sustainml::common::TopicCollection::get()[common::ML_MODEL_METADATA].second.c_str(),
            &(*listener_model_metadata_queue_), opts);

    initialize_subscription(sustainml::common::TopicCollection::get()[common::APP_REQUIREMENT].first.c_str(),
            sustainml::common::TopicCollection::get()[common::APP_REQUIREMENT].second.c_str(),
            &(*listener_app_requirements_queue_), opts);

    initialize_subscription(sustainml::common::TopicCollection::get()[common::HW_CONSTRAINT].first.c_str(),
            sustainml::common::TopicCollection::get()[common::HW_CONSTRAINT].second.c_str(),
            &(*listener_hw_constraints_queue_), opts);

    initialize_publication(sustainml::common::TopicCollection::get()[common::ML_MODEL].first.c_str(),
            sustainml::common::TopicCollection::get()[common::ML_MODEL].second.c_str(),
            opts);

    // Baselines topics
    initialize_subscription(sustainml::common::TopicCollection::get()[common::ML_MODEL_BASELINE].first.c_str(),
            sustainml::common::TopicCollection::get()[common::ML_MODEL].second.c_str(),
            &(*listener_mlmodel_queue_), opts);

    initialize_subscription(sustainml::common::TopicCollection::get()[common::HW_RESOURCES_BASELINE].first.c_str(),
            sustainml::common::TopicCollection::get()[common::HW_RESOURCE].second.c_str(),
            &(*listener_hw_queue_), opts);

    initialize_subscription(sustainml::common::TopicCollection::get()[common::CARBON_FOOTPRINT_BASELINE].first.c_str(),
            sustainml::common::TopicCollection::get()[common::CARBON_FOOTPRINT].second.c_str(),
            &(*listener_carbon_footprint_queue_), opts);
}

void MLModelNode::publish_to_user(
        const types::TaskId& task_id,
        const std::vector<std::pair<int, void*>> input_samples)
{
    //! Expected inputs are the number of reader minus the control reader
    if (input_samples.size() == ExpectedInputSamples::MAX)
    {
        auto& user_listener_args = user_listener_.create_and_get_user_cb_args(task_id);

        size_t samples_retrieved{0};
        common::pair_queue_id_with_sample_type(
            input_samples,
            user_listener_args,
            ExpectedInputSamples::MAX,
            samples_retrieved);

        types::NodeTaskOutputData<MLModel>* task_data_cache;

        {
            std::lock_guard<std::mutex> lock (mtx_);

            auto& status = std::get<TASK_STATUS_DATA>(user_listener_args);
            auto& output = std::get<TASK_OUTPUT_DATA>(user_listener_args);

            task_data_cache = task_data_pool_->get_new_cache_nts();

            status = &task_data_cache->node_status;
            output = &task_data_cache->output_data;
        }

        //! TODO: Manage task statuses individually

        if (status() != Status::NODE_RUNNING)
        {
            status(Status::NODE_RUNNING);
            publish_node_status();
        }

        user_listener_.invoke_user_cb(task_id, core::helper::gen_seq<MLModelCallable::size>{});

        //! Ensure task_id is forwarded to the output
        task_data_cache->output_data.task_id(task_id);

        if (task_data_cache->node_status.node_status() != Status::NODE_ERROR)
        {
            status(Status::NODE_IDLE);
        }
        else
        {
            status(Status::NODE_ERROR);
        }

        publish_node_status();
        writers()[OUTPUT_WRITER_IDX]->write(task_data_cache->output_data.get_impl());

        listener_model_metadata_queue_->remove_element_by_taskid(task_id);
        listener_app_requirements_queue_->remove_element_by_taskid(task_id);
        listener_hw_constraints_queue_->remove_element_by_taskid(task_id);

        {
            std::lock_guard<std::mutex> lock (mtx_);
            task_data_pool_->release_cache_nts(task_data_cache);
            user_listener_.remove_task_args(task_id);
        }
    }
    else
    {
        EPROSIMA_LOG_ERROR(MLMODEL_NODE, "Input size mismatch");
    }
}

} // ml_model_module
} // sustainml
