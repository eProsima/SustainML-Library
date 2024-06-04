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
 * @file MLModelMetadataNode.cpp
 */

#include <sustainml_cpp/nodes/MLModelMetadataNode.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>

#include <common/Common.hpp>
#include <core/Options.hpp>
#include <core/QueuedNodeListener.hpp>
#include <types/typesImpl.h>

using namespace types;

namespace sustainml {
namespace ml_model_module {

MLModelMetadataNode::MLModelMetadataNode(
        MLModelMetadataTaskListener& user_listener)
    : Node(common::ML_MODEL_METADATA_NODE)
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

MLModelMetadataNode::MLModelMetadataNode(
        MLModelMetadataTaskListener& user_listener,
        sustainml::core::Options opts)
    : Node(common::ML_MODEL_METADATA_NODE, opts)
    , user_listener_(user_listener)
{
    init(opts);
}

MLModelMetadataNode::~MLModelMetadataNode()
{

}

void MLModelMetadataNode::init (
        const sustainml::core::Options& opts)
{
    listener_user_input_queue_.reset(new core::QueuedNodeListener<UserInput>(this));

    task_data_pool_.reset(new utils::SamplePool<types::NodeTaskOutputData<MLModelMetadata>>(opts));

    initialize_subscription(sustainml::common::TopicCollection::get()[common::USER_INPUT].first.c_str(),
            sustainml::common::TopicCollection::get()[common::USER_INPUT].second.c_str(),
            &(*listener_user_input_queue_), opts);

    initialize_publication(sustainml::common::TopicCollection::get()[common::ML_MODEL_METADATA].first.c_str(),
            sustainml::common::TopicCollection::get()[common::ML_MODEL_METADATA].second.c_str(),
            opts);
}

void MLModelMetadataNode::publish_to_user(
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

        types::NodeTaskOutputData<MLModelMetadata>* task_data_cache;

        {
            std::lock_guard<std::mutex> lock (mtx_);

            auto& status = std::get<TASK_STATUS_DATA>(user_listener_args);
            auto& output = std::get<TASK_OUTPUT_DATA>(user_listener_args);

            task_data_cache = task_data_pool_->get_new_cache_nts();

            status = &task_data_cache->node_status;
            output = &task_data_cache->output_data;
        }

        //! TODO: Manage task statuses individually

        if (status() != NODE_RUNNING)
        {
            status(NODE_RUNNING);
            publish_node_status();
        }

        user_listener_.invoke_user_cb(task_id, core::helper::gen_seq<MLModelMetadataCallable::size>{});

        //! Ensure task_id is forwarded to the output
        task_data_cache->output_data.task_id(task_id);

        if (task_data_cache->node_status.node_status() != NODE_ERROR)
        {
            status(NODE_IDLE);
        }
        else
        {
            status(NODE_ERROR);
        }

        publish_node_status();
        writers()[OUTPUT_WRITER_IDX]->write(task_data_cache->output_data.get_impl());

        listener_user_input_queue_->remove_element_by_taskid(task_id);

        {
            std::lock_guard<std::mutex> lock (mtx_);
            task_data_pool_->release_cache_nts(task_data_cache);
            user_listener_.remove_task_args(task_id);
        }
    }
    else
    {
        EPROSIMA_LOG_ERROR(MLMODELMETADATA_NODE, "Input size mismatch");
    }
}

} // ml_model_module
} // sustainml
