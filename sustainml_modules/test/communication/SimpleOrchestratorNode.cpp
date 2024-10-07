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

#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>

struct SimpleOrchestratorNodeHandle : public sustainml::orchestrator::OrchestratorNodeHandle
{
    using DataCollection = std::map<sustainml::NodeID, std::pair<Status, int>>;

    void on_new_node_output(
            const sustainml::NodeID& id,
            void* data)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        node_data_received_[id].second++;
        cv_.notify_one();
        std::cout << "NodeOutput received " << (int)id << std::endl;
    }

    void on_node_status_change(
            const sustainml::NodeID& id,
            const types::NodeStatus& status)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        node_data_received_[id].first = status.node_status();
        cv_.notify_one();
    }

    void prepare_expected_data(
            const DataCollection& expected_data)
    {
        expected_node_data_ = expected_data;
    }

    template<class _Rep,
            class _Period
            >
    bool wait_for_data(
            const std::chrono::duration<_Rep, _Period>& max_wait)
    {
        std::unique_lock<std::mutex> lock(cv_mutex_);
        cv_.wait_for(lock, max_wait, [this]() -> bool
                {
                    for (auto& x : node_data_received_)
                    {
                        std::cout << "Arrived " << (int)x.first << (int)x.second.first << x.second.second << std::endl;
                    }
                    for (auto& x : expected_node_data_)
                    {
                        std::cout << "Expected " << (int)x.first << (int)x.second.first << x.second.second << std::endl;
                    }
                    return expected_node_data_ == node_data_received_;
                });

        return expected_node_data_ == node_data_received_;
    }

private:

    std::mutex cv_mutex_;
    std::condition_variable cv_;

    // map of NodeID to pair of <Status, Output counter>
    std::mutex mtx_;
    DataCollection node_data_received_;
    DataCollection expected_node_data_;
};

int main(
        int argc,
        char** argv)
{
    SimpleOrchestratorNodeHandle* sonh = new SimpleOrchestratorNodeHandle();

    // NodeID, <Node Status, Output counter>
    SimpleOrchestratorNodeHandle::DataCollection expected_data_nodes_ready =
    {
        {sustainml::NodeID::ID_ML_MODEL_METADATA, {(Status)2, 0}},
        {sustainml::NodeID::ID_ML_MODEL, {(Status)2, 0}},
        {sustainml::NodeID::ID_HW_RESOURCES, {(Status)2, 0}},
        {sustainml::NodeID::ID_CARBON_FOOTPRINT, {(Status)2, 0}},
        {sustainml::NodeID::ID_HW_CONSTRAINTS, {(Status)2, 0}},
        {sustainml::NodeID::ID_APP_REQUIREMENTS, {(Status)2, 0}}
    };

    // NodeID, <Node Status, Output counter>
    SimpleOrchestratorNodeHandle::DataCollection expected_data_task_executed =
    {
        {sustainml::NodeID::ID_ML_MODEL_METADATA, {(Status)2, 1}},
        {sustainml::NodeID::ID_ML_MODEL, {(Status)2, 1}},
        {sustainml::NodeID::ID_HW_RESOURCES, {(Status)2, 1}},
        {sustainml::NodeID::ID_CARBON_FOOTPRINT, {(Status)2, 1}},
        {sustainml::NodeID::ID_HW_CONSTRAINTS, {(Status)2, 1}},
        {sustainml::NodeID::ID_APP_REQUIREMENTS, {(Status)2, 1}}
    };

    sonh->prepare_expected_data(expected_data_nodes_ready);

    sustainml::orchestrator::OrchestratorNode orchestrator(*sonh);

    // First, wait for all nodes to be ready
    sonh->wait_for_data(std::chrono::hours(24));

    sonh->prepare_expected_data(expected_data_task_executed);

    auto new_task = orchestrator.prepare_new_task();
    new_task.second->task_id(new_task.first);
    new_task.second->modality("video");
    new_task.second->problem_definition("Classify cars in a video sequence.");

    orchestrator.start_task(new_task.first, new_task.second);

    // Now all nodes should have publish one output i.e the one for {1,1}
    sonh->wait_for_data(std::chrono::hours(24));

    return 0;
}
