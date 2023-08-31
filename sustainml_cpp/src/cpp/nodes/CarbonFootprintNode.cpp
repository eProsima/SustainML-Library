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
 * @file CarbonFootprintNode.cpp
 */

#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>

#include <common/Common.hpp>
#include <core/QueuedNodeListener.hpp>

namespace sustainml {
namespace co2_tracker_module {

    CarbonFootprintNode::CarbonFootprintNode() : Node(common::CO2_TRACKER_NODE)
    {
        listener_ml_model_queue_.reset(new core::QueuedNodeListener<MLModel>(this));
        listener_hw_queue_.reset(new core::QueuedNodeListener<HWResource>(this));
        listener_user_input_queue_.reset(new core::QueuedNodeListener<UserInput>(this));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::ML_MODEL].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::ML_MODEL].second.c_str(),
                                &(*listener_ml_model_queue_));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::HW_RESOURCE].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::HW_RESOURCE].second.c_str(),
                                &(*listener_hw_queue_));

        initialize_subscription(sustainml::common::TopicCollection::get()[common::USER_INPUT].first.c_str(),
                                sustainml::common::TopicCollection::get()[common::USER_INPUT].second.c_str(),
                                &(*listener_user_input_queue_));

        initialize_publication(sustainml::common::TopicCollection::get()[common::CO2_FOOTPRINT].first.c_str(),
                               sustainml::common::TopicCollection::get()[common::CO2_FOOTPRINT].second.c_str());
    }

    CarbonFootprintNode::~CarbonFootprintNode()
    {

    }

    void CarbonFootprintNode::publish_to_user(const std::vector<std::pair<int,void*>> input_samples)
    {
        //! Expected inputs are the number of reader minus the control reader
        if (input_samples.size() == ExpectedInputSamples::MAX)
        {
            size_t samples_retrieved{0};
            common::pair_queue_id_with_sample_type(
                    input_samples,
                    Callable::get_user_cb_args(),
                    ExpectedInputSamples::MAX,
                    samples_retrieved);

            int task_id{-1};
            auto first_sample_ptr = std::get<USER_INPUT_SAMPLE>(Callable::get_user_cb_args());

            if (nullptr != first_sample_ptr)
            {
                task_id = first_sample_ptr->task_id();
            }

            if (task_id == common::INVALID_ID)
            {
                EPROSIMA_LOG_ERROR(CO2_NODE, "Error Retrieving the task_id of a sample");
                return;
            }

            {
                std:std::unique_lock<std::mutex> lock (mtx_);
                task_data_.insert({task_id, {NodeStatus(), CO2Footprint()}});

                auto& status = std::get<TASK_STATUS_DATA>(Callable::get_user_cb_args());
                auto& output = std::get<TASK_OUTPUT_DATA>(Callable::get_user_cb_args());

                status = &task_data_[task_id].first;
                output = &task_data_[task_id].second;
            }

            //! TODO: Manage task statuses individually

            if (node_status_.node_status() != NODE_RUNNING)
            {
                node_status_.node_status(NODE_RUNNING);
                publish_node_status();
            }

            Callable::invoke_user_cb(core::helper::gen_seq<size>{});

            //! Ensure task_id is forwarded to the output
            task_data_[task_id].second.task_id(task_id);

            writers_[OUTPUT_WRITER_IDX]->write(&task_data_[task_id].second);

            listener_ml_model_queue_->remove_element_by_taskid(task_id);
            listener_hw_queue_->remove_element_by_taskid(task_id);
            listener_user_input_queue_->remove_element_by_taskid(task_id);

            {
                std::unique_lock<std::mutex> lock (mtx_);
                auto task_it = task_data_.erase(task_id);
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(CO2_NODE, "Input size mismatch");
        }
    }

} // co2_tracker_module
} // sustainml
