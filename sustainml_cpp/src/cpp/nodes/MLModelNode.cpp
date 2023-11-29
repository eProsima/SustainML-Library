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
#include <types/typesImpl.h>

using namespace types;

namespace sustainml {
namespace ml_model_provider_module {

    MLModelNode::MLModelNode(
            MLModelTaskListener& user_listener)
            : Node(common::ML_MODEL_NODE)
            , user_listener_(user_listener)
    {
        sustainml::core::Options opts;
        opts.rqos.resource_limits().max_instances = 500;
        opts.rqos.resource_limits().max_samples_per_instance = 1;
        opts.wqos.resource_limits().max_instances = 500;
        opts.wqos.resource_limits().max_samples_per_instance = 1;

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

    MLModelNode::~MLModelNode()
    {

    }

    void MLModelNode::init (const sustainml::core::Options& opts)
    {
        listener_enc_task_queue_.reset(new core::QueuedNodeListener<EncodedTask>(this));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::ENCODED_TASK].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::ENCODED_TASK].second.c_str(),
                                &(*listener_enc_task_queue_), opts);

        initialize_publication(sustainml::common::TopicCollection::get()[common::ML_MODEL].first.c_str(),
                               sustainml::common::TopicCollection::get()[common::ML_MODEL].second.c_str(),
                               opts);
    }

    void MLModelNode::publish_to_user(const std::vector<std::pair<int,void*>> input_samples)
    {
        //! Expected inputs are the number of reader minus the control reader
        if (input_samples.size() == ExpectedInputSamples::MAX)
        {
            auto& user_listener_args = user_listener_.get_user_cb_args();

            size_t samples_retrieved{0};
            common::pair_queue_id_with_sample_type(
                    input_samples,
                    user_listener_args,
                    ExpectedInputSamples::MAX,
                    samples_retrieved);

            int task_id{-1};
            auto first_sample_ptr = std::get<ENCODED_TASK_SAMPLE>(user_listener_args);

            if (nullptr != first_sample_ptr)
            {
                task_id = first_sample_ptr->task_id();
            }

            if (task_id == common::INVALID_ID)
            {
                EPROSIMA_LOG_ERROR(MLMODEL_NODE, "Error Retrieving the task_id of a sample");
                return;
            }

            {
                std::unique_lock<std::mutex> lock (mtx_);
                task_data_.insert({task_id, {NodeStatus(), MLModel()}});

                auto& status = std::get<TASK_STATUS_DATA>(user_listener_args);
                auto& output = std::get<TASK_OUTPUT_DATA>(user_listener_args);

                status = &task_data_[task_id].first;
                output = &task_data_[task_id].second;
            }

            //! TODO: Manage task statuses individually

            if (status() != NODE_RUNNING)
            {
                status(NODE_RUNNING);
                publish_node_status();
            }

            user_listener_.invoke_user_cb(core::helper::gen_seq<MLModelCallable::size>{});

            //! Ensure task_id is forwarded to the output
            task_data_[task_id].second.task_id(task_id);

            writers()[OUTPUT_WRITER_IDX]->write(task_data_[task_id].second.get_impl());

            listener_enc_task_queue_->remove_element_by_taskid(task_id);

            {
                std::unique_lock<std::mutex> lock (mtx_);
                task_data_.erase(task_id);
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(MLMODEL_NODE, "Input size mismatch");
        }
    }

} // ml_model_provider_module
} // sustainml
