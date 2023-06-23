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

    CarbonFootprintNode::CarbonFootprintNode() : Node("CO2_TRACKER_NODE")
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
        if (input_samples.size() == ExpectedInputs::MAX)
        {
            std::tuple<MLModel*, UserInput*, HWResource*> user_inputs{nullptr, nullptr, nullptr};

            common::pair_queue_id_with_sample_type(input_samples, user_inputs);

            int task_id = std::get<0>(user_inputs)->task_id();

            {
                std:std::unique_lock<std::mutex> lock (mtx_);
                user_data_.insert({task_id, {NodeStatus(), CO2Footprint()}});
            }

            //! TODO: Manage task statuses individually

            if (node_status_.node_status() != NODE_RUNNING)
            {
                node_status_.node_status(NODE_RUNNING);
                publish_node_status();
            }

            user_callback_(*std::get<0>(user_inputs),
                           *std::get<1>(user_inputs),
                           *std::get<2>(user_inputs),
                           user_data_[task_id].first, user_data_[task_id].second);

            //! TODO improve indexing
            writers_[1]->write(&user_data_[task_id].second);

            listener_ml_model_queue_->remove_element_by_taskid(task_id);
            listener_hw_queue_->remove_element_by_taskid(task_id);
            listener_user_input_queue_->remove_element_by_taskid(task_id);

            {
                std::unique_lock<std::mutex> lock (mtx_);
                auto task_it = user_data_.erase(task_id);
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(CO2_NODE, "Input size mismatch");
        }
    }

} // co2_tracker_module
} // sustainml
