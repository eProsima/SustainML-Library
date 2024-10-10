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
 * @file OrchestratorNode.hpp
 */

#ifndef SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP
#define SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP

#include <condition_variable>
#include <memory>
#include <mutex>

#include <sustainml_cpp/core/Constants.hpp>
#include <sustainml_cpp/types/types.h>

#include <fastdds/dds/domain/DomainParticipantListener.hpp>

namespace eprosima {
namespace fastdds {
namespace dds {

class DomainParticipant;
class Topic;
class Publisher;
class DataWriter;

} // namespace dds
} // namespace fastdds
} // namespace eprosima

namespace sustainml {
namespace orchestrator {

class ModuleNodeProxy;
template <typename ... Args> class TaskDB;
class TaskManager;

/**
 * @brief This class is meant for the user to implement
 * the callbacks when the OrchestratorNode receives new data.
 *
 */
struct OrchestratorNodeHandle
{
    OrchestratorNodeHandle() = default;
    virtual ~OrchestratorNodeHandle() = default;

    /**
     * @brief Callback to notify the user that new output data is available in one node.
     * @param   id identifier of the node that triggered the new output
     * @param data generic pointer to the output data
     */
    virtual void on_new_node_output(
            const NodeID& id,
            void* data) = 0;

    /**
     * @brief Callback to notify the user that the status of a node has changed.
     * @param     id identifier of the node that triggered the new status
     * @param status pointer to the status data
     */
    virtual void on_node_status_change(
            const NodeID& id,
            const types::NodeStatus& status) = 0;
};

class OrchestratorNode
{
    friend class ModuleNodeProxy;

public:

    using TaskDB_t =  TaskDB<
        types::AppRequirements,
        types::CO2Footprint,
        types::HWConstraints,
        types::HWResource,
        types::MLModelMetadata,
        types::MLModel,
        types::UserInput>;

    OrchestratorNode(
            OrchestratorNodeHandle& handler,
            uint32_t domain = 0);

    ~OrchestratorNode();

    /**
     * @brief Get the task data from DB given the task_id and node identifier.
     * @param [in] task_id id identifier of the task that has new data available
     * @param [in] node_id id identifier of the node that triggered the new status
     * @param [in,out]  data pointer that will be redirected to the data
     */
    RetCode_t get_task_data(
            const types::TaskId& task_id,
            const NodeID& node_id,
            void*& data);

    /**
     * @brief Get the node status from DB given node identifier.
     * @param [in] node_id id identifier of the node that triggered the new status
     * @param [in,out]  status pointer that will be redirected to the status data
     *
     */
    RetCode_t get_node_status(
            const NodeID& node_id,
            const types::NodeStatus*& status);

    /**
     * @brief This method reserves a new Task cache in the DB and returns the place
     * where to fill the UserInput entry structure.
     * @note It must be called before start_task()
     */
    std::pair<types::TaskId, types::UserInput*> prepare_new_task();

    /**
     * @brief This method reserves a new Task cache in the DB and returns the place
     * where to fill the UserInput entry structure.
     * @param [in] task_id identifier of the previous task from which to iterate
     * @note It must be called before start_task()
     */
    std::pair<types::TaskId, types::UserInput*> prepare_new_iteration(
            const types::TaskId& task_id);

    /**
     * @brief This method triggers a new task with a previously prepared task_id and
     * a pointer to the UserInput data structure.
     * @param [in] task_id id task identifier of the desired task
     * @param [in]      ui pointer to the user input data
     */
    bool start_task(
            const types::TaskId& task_id,
            types::UserInput* ui);

    /**
     * @brief This method triggers a new iteration on a previous task.
     * @param [in] task_id id task identifier of the desired task
     * @param [in]      ui pointer to the user input data
     */
    bool start_iteration(
            const types::TaskId& task_id,
            types::UserInput* ui);

    /**
     * @brief This method sends the specified Node Control command to the related node.
     * @param [in] cmd control command to send
     */
    void send_control_command(
            const types::NodeControl& cmd);

    /**
     * @brief Public method to get the mutex in order to correctly synchronise user
     * handle calls.
     */
    inline std::mutex& get_mutex()
    {
        return mtx_;
    }

    /**
     * @brief Used to retrieve the associated OrchestratorNodeHandle.
     */
    inline OrchestratorNodeHandle* get_handler() const
    {
        return handler_;
    }

    /**
     * @brief Used to retrieve the associated OrchestratorNodeHandle.
     */
    void print_db();

    /**
     * @brief Called by the user to run the run.
     */
    void spin();

    /**
     * @brief Remove all Fast DDS entities and clean up the OrchestratorNode and OrchestratorNodeHandle.
     *
     */
    void destroy();

    /**
     * @brief Stops the execution of the node.
     */
    void terminate();

protected:

    /**
     * @brief Used for intializing the Orchestrator
     */
    bool init();

    /**
     * @brief Publishes node baselines
     */
    void publish_baselines(
            const types::TaskId& task_id);

    uint32_t domain_;

    OrchestratorNodeHandle* handler_;

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Topic* control_topic_;
    eprosima::fastdds::dds::Topic* status_topic_;
    eprosima::fastdds::dds::Topic* user_input_topic_;

    eprosima::fastdds::dds::Publisher* pub_;
    eprosima::fastdds::dds::Subscriber* sub_;

    eprosima::fastdds::dds::DataWriter* control_writer_;
    eprosima::fastdds::dds::DataWriter* user_input_writer_;

    std::array<ModuleNodeProxy*, (size_t)NodeID::MAX> node_proxies_;
    std::mutex proxies_mtx_;

    std::shared_ptr<TaskDB_t> task_db_;

    TaskManager* task_man_;

    std::mutex mtx_;

    std::atomic_bool initialized_{false};
    std::atomic_bool terminated_{false};
    std::condition_variable initialization_cv_;

    /**
     * @brief This class implements the callbacks for the DomainParticipant
     */
    class OrchestratorParticipantListener : public eprosima::fastdds::dds::DomainParticipantListener
    {
    public:

        OrchestratorParticipantListener(
                OrchestratorNode* orchestrator);

        virtual void on_participant_discovery(
                eprosima::fastdds::dds::DomainParticipant* participant,
                eprosima::fastdds::rtps::ParticipantDiscoveryStatus reason,
                const eprosima::fastdds::rtps::ParticipantBuiltinTopicData& info,
                bool& should_be_ignored);

    private:

        OrchestratorNode* orchestrator_{nullptr};

    };

    std::condition_variable spin_cv_;
    std::atomic_bool terminate_;

    std::unique_ptr<OrchestratorParticipantListener> participant_listener_;

};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP

