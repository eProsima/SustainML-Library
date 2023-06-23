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
 * @file TaskEncoderNode.cpp
 */

#include <sustainml_cpp/nodes/TaskEncoderNode.hpp>

#include <common/Common.hpp>
#include <core/QueuedNodeListener.hpp>

namespace sustainml {
namespace ml_task_encoding_module {

    TaskEncoderNode::TaskEncoderNode() : Node("TASK_ENCODER_NODE")
    {
        listener_user_input_queue_.reset(new core::QueuedNodeListener<UserInput>(this));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::USER_INPUT].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::USER_INPUT].second.c_str(),
                                &(*listener_user_input_queue_));

        initialize_publication(sustainml::common::TopicCollection::get()[common::ENCODED_TASK].first.c_str(),
                               sustainml::common::TopicCollection::get()[common::ENCODED_TASK].second.c_str());
    }

    TaskEncoderNode::~TaskEncoderNode()
    {

    }

    void TaskEncoderNode::publish_to_user(const std::vector<std::pair<int,void*>> input_samples)
    {
        //! Expected inputs are the number of reader minus the control reader
        if (input_samples.size() == ExpectedInputs::MAX)
        {
            std::tuple<UserInput*> user_inputs{nullptr};

            common::pair_queue_id_with_sample_type(input_samples, user_inputs);

            int task_id = std::get<0>(user_inputs)->task_id();

            {
                std:std::unique_lock<std::mutex> lock (mtx_);
                user_data_.insert({task_id, {NodeStatus(), EncodedTask()}});
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

            listener_user_input_queue_->remove_element_by_taskid(task_id);

            //! TODO improve deletion condition
            {
                std::unique_lock<std::mutex> lock (mtx_);
                auto task_it = user_data_.erase(task_id);
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(TASKENC_NODE, "Input size mismatch");
        }
    }

} // ml_task_encoding_module
} // sustainml
