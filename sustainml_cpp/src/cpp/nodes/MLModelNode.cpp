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

#include <common/Common.hpp>
#include <core/QueuedNodeListener.hpp>

namespace sustainml {
namespace ml_model_provider_module {

    MLModelNode::MLModelNode() : Node("ML_MODEL_NODE")
    {
        listener_enc_task_queue_.reset(new core::QueuedNodeListener<EncodedTask>(this));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::ENCODED_TASK].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::ENCODED_TASK].second.c_str(),
                                &(*listener_enc_task_queue_));

        initialize_publication(sustainml::common::TopicCollection::get()[common::ML_MODEL].first.c_str(),
                               sustainml::common::TopicCollection::get()[common::ML_MODEL].second.c_str());
    }

    MLModelNode::~MLModelNode()
    {

    }

    void MLModelNode::publish_to_user(const std::vector<std::pair<int,void*>> input_samples)
    {
        //! Expected inputs are the number of reader minus the control reader
        if (input_samples.size() == ExpectedInputs::MAX)
        {
            std::tuple<EncodedTask*> user_inputs{nullptr};

            common::pair_queue_id_with_sample_type(input_samples, user_inputs);

            int task_id = std::get<0>(user_inputs)->task_id();

            {
                std:std::unique_lock<std::mutex> lock (mtx_);
                user_data_.insert({task_id, {NodeStatus(), MLModel()}});
            }

            //! TODO: Manage task statuses individually

            if (node_status_.node_status() != NODE_RUNNING)
            {
                node_status_.node_status(NODE_RUNNING);
                publish_node_status();
            }

            user_callback_(*std::get<0>(user_inputs),
                           user_data_[task_id].first, user_data_[task_id].second);

            //! TODO improve indexing
            writers_[1]->write(&user_data_[task_id].second);

            listener_enc_task_queue_->remove_element_by_taskid(task_id);

            {
                std::unique_lock<std::mutex> lock (mtx_);
                auto task_it = user_data_.erase(task_id);
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(MLMODEL_NODE, "Input size mismatch");
        }
    }

} // ml_model_provider_module
} // sustainml
