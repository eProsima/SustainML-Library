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

#include "BlackboxTests.hpp"

#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>

struct TestOrchestratorNodeHandle : public orchestrator::OrchestratorNodeHandle
{
    using DataCollection = std::map<NodeID, std::pair<Status, int>>;

    void on_new_node_output(
            const NodeID& id,
            void* data)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        node_data_received_[id].second++;
        cv_.notify_one();
    }

    void on_node_status_change(
            const NodeID& id,
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
                        std::cout << "Arrived " << (int)x.first << x.second.first << x.second.second << std::endl;
                    }
                    for (auto& x : expected_node_data_)
                    {
                        std::cout << "Expected " << (int)x.first << x.second.first << x.second.second << std::endl;
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

static TestOrchestratorNodeHandle::DataCollection nodes_ready_expected_data =
{
    {NodeID::ID_ML_MODEL_METADATA, {NODE_IDLE, 0}},
    {NodeID::ID_ML_MODEL, {NODE_IDLE, 0}},
    {NodeID::ID_HW_RESOURCES, {NODE_IDLE, 0}},
    {NodeID::ID_CARBON_FOOTPRINT, {NODE_IDLE, 0}},
    {NodeID::ID_APP_REQUIREMENTS, {NODE_IDLE, 0}},
    {NodeID::ID_HW_CONSTRAINTS, {NODE_IDLE, 0}}
};

TEST(OrchestratorNode, OrchestratorInitializesProperlyWhenNodesAreALive)
{
    std::shared_ptr<TestOrchestratorNodeHandle> tonh = std::make_shared<TestOrchestratorNodeHandle>();

    tonh->prepare_expected_data(nodes_ready_expected_data);

    orchestrator::OrchestratorNode orchestrator(tonh);

    MLModelMetadataManagedNode ml_met_node;
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    AppRequirementsManagedNode app_req_node;

    co2_node.start();
    hw_node.start();
    ml_node.start();
    ml_met_node.start();
    app_req_node.start();
    hw_cons_node.start();

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(5)));
}

TEST(OrchestratorNode, AlateJoinerOrchestratorInitializesProperly)
{
    std::shared_ptr<TestOrchestratorNodeHandle> tonh = std::make_shared<TestOrchestratorNodeHandle>();

    TestOrchestratorNodeHandle::DataCollection expected_data =
    {
        {NodeID::ID_ML_MODEL_METADATA, {NODE_IDLE, 0}},
        {NodeID::ID_ML_MODEL, {NODE_IDLE, 0}},
        {NodeID::ID_HW_RESOURCES, {NODE_IDLE, 0}},
        {NodeID::ID_CARBON_FOOTPRINT, {NODE_IDLE, 0}},
        {NodeID::ID_HW_CONSTRAINTS, {NODE_IDLE, 0}},
        {NodeID::ID_APP_REQUIREMENTS, {NODE_IDLE, 0}}
    };

    tonh->prepare_expected_data(expected_data);

    MLModelMetadataManagedNode ml_met_node;
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    AppRequirementsManagedNode app_req_node;

    co2_node.start();
    hw_node.start();
    ml_node.start();
    ml_met_node.start();
    app_req_node.start();
    hw_cons_node.start();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    orchestrator::OrchestratorNode orchestrator(tonh);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(5)));
}

TEST(OrchestratorNode, OrchestratorReceivesNodeOutputs)
{
    std::shared_ptr<TestOrchestratorNodeHandle> tonh = std::make_shared<TestOrchestratorNodeHandle>();

    orchestrator::OrchestratorNode orchestrator(tonh);

    MLModelMetadataManagedNode te_node;
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    AppRequirementsManagedNode app_req_node;

    co2_node.start();
    hw_node.start();
    ml_node.start();
    te_node.start();
    hw_cons_node.start();
    app_req_node.start();

    // Wait for all nodes to be idle
    tonh->prepare_expected_data(nodes_ready_expected_data);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(3)));

    TestOrchestratorNodeHandle::DataCollection test_expected_data =
    {
        {NodeID::ID_ML_MODEL_METADATA, {NODE_IDLE, 1}},
        {NodeID::ID_ML_MODEL, {NODE_IDLE, 1}},
        {NodeID::ID_HW_RESOURCES, {NODE_IDLE, 1}},
        {NodeID::ID_CARBON_FOOTPRINT, {NODE_IDLE, 1}},
        {NodeID::ID_HW_CONSTRAINTS, {NODE_IDLE, 1}},
        {NodeID::ID_APP_REQUIREMENTS, {NODE_IDLE, 1}}
    };

    tonh->prepare_expected_data(test_expected_data);

    auto task = orchestrator.prepare_new_task();

    task.second->task_id(task.first);
    task.second->problem_short_description("Test_Task_name");
    task.second->problem_definition("Test");

    orchestrator.start_task(task.first, task.second);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(5)));
}

TEST(OrchestratorNode, OrchestratorGetTaskData)
{
    std::shared_ptr<TestOrchestratorNodeHandle> tonh = std::make_shared<TestOrchestratorNodeHandle>();

    orchestrator::OrchestratorNode orchestrator(tonh);

    MLModelMetadataManagedNode te_node;
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    AppRequirementsManagedNode app_req_node;

    co2_node.start();
    hw_node.start();
    ml_node.start();
    te_node.start();
    hw_cons_node.start();
    app_req_node.start();

    // Wait for all nodes to be idle
    tonh->prepare_expected_data(nodes_ready_expected_data);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(5)));

    TestOrchestratorNodeHandle::DataCollection test_expected_data =
    {
        {NodeID::ID_ML_MODEL_METADATA, {NODE_IDLE, 2}},
        {NodeID::ID_ML_MODEL, {NODE_IDLE, 2}},
        {NodeID::ID_HW_RESOURCES, {NODE_IDLE, 2}},
        {NodeID::ID_CARBON_FOOTPRINT, {NODE_IDLE, 2}},
        {NodeID::ID_HW_CONSTRAINTS, {NODE_IDLE, 2}},
        {NodeID::ID_APP_REQUIREMENTS, {NODE_IDLE, 2}}
    };

    tonh->prepare_expected_data(test_expected_data);

    auto task = orchestrator.prepare_new_task();

    task.second->task_id(task.first);
    task.second->problem_short_description("Test_Task0_name");
    task.second->problem_definition("Test_Task0");

    orchestrator.start_task(task.first, task.second);

    task = orchestrator.prepare_new_task();

    task.second->task_id(task.first);
    task.second->problem_short_description("Test_Task1_name");
    task.second->problem_definition("Test_Task1");

    orchestrator.start_task(task.first, task.second);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(10)));
    void* enc_task = nullptr;
    void* hw = nullptr;
    ASSERT_EQ(orchestrator.get_task_data({1,1}, NodeID::ID_ML_MODEL_METADATA, enc_task), RetCode_t::RETCODE_OK);
    ASSERT_EQ(((types::MLModelMetadata*)enc_task)->task_id().problem_id(), 1);
    ASSERT_EQ(orchestrator.get_task_data({2,2}, NodeID::ID_ML_MODEL_METADATA, enc_task), RetCode_t::RETCODE_OK);
    ASSERT_EQ(((types::MLModelMetadata*)enc_task)->task_id().problem_id(), 2);
    ASSERT_EQ(orchestrator.get_task_data({2,2}, NodeID::ID_HW_RESOURCES, hw), RetCode_t::RETCODE_OK);
    ASSERT_EQ(((types::HWResource*)hw)->task_id().problem_id(), 2);
}

TEST(OrchestratorNode, OrchestratorGetNodeStatus)
{
    std::shared_ptr<TestOrchestratorNodeHandle> tonh = std::make_shared<TestOrchestratorNodeHandle>();

    orchestrator::OrchestratorNode orchestrator(tonh);

    MLModelMetadataManagedNode te_node;
    MLModelManagedNode ml_node;
    HWResourcesManagedNode hw_node;
    CarbonFootprintManagedNode co2_node;
    HWConstraintsManagedNode hw_cons_node;
    AppRequirementsManagedNode app_req_node;

    co2_node.start();
    hw_node.start();
    ml_node.start();
    te_node.start();
    hw_cons_node.start();
    app_req_node.start();

    // Wait for all nodes to be idle
    tonh->prepare_expected_data(nodes_ready_expected_data);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(3)));

    TestOrchestratorNodeHandle::DataCollection test_expected_data =
    {
        {NodeID::ID_ML_MODEL_METADATA, {NODE_IDLE, 1}},
        {NodeID::ID_ML_MODEL, {NODE_IDLE, 1}},
        {NodeID::ID_HW_RESOURCES, {NODE_IDLE, 1}},
        {NodeID::ID_CARBON_FOOTPRINT, {NODE_IDLE, 1}},
        {NodeID::ID_HW_CONSTRAINTS, {NODE_IDLE, 1}},
        {NodeID::ID_APP_REQUIREMENTS, {NODE_IDLE, 1}}
    };

    tonh->prepare_expected_data(test_expected_data);

    auto task = orchestrator.prepare_new_task();

    task.second->task_id(task.first);
    task.second->problem_short_description("Test_Task0_name");
    task.second->problem_definition("Test_Task0");

    orchestrator.start_task(task.first, task.second);

    ASSERT_TRUE(tonh->wait_for_data(std::chrono::seconds(10)));
    const types::NodeStatus* status;
    orchestrator.get_node_status(NodeID::ID_ML_MODEL_METADATA, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
    orchestrator.get_node_status(NodeID::ID_ML_MODEL, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
    orchestrator.get_node_status(NodeID::ID_HW_RESOURCES, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
    orchestrator.get_node_status(NodeID::ID_CARBON_FOOTPRINT, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
    orchestrator.get_node_status(NodeID::ID_HW_CONSTRAINTS, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
    orchestrator.get_node_status(NodeID::ID_APP_REQUIREMENTS, status);
    ASSERT_EQ(status->node_status(), NODE_IDLE);
}
