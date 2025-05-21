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
#include <sustainml_cpp/types/types.hpp>

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
namespace core {
class RequestReplier;
} // namespace core
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

    /**
     * @brief Construct a new Orchestrator Node object
     *
     * @param handler OrchestratorNodeHandle object to handle the callbacks
     * @param domain Domain ID to use for the DDS entities
     *
     * @note The deletion of the handler is responsibility of the user.
     */
    OrchestratorNode(
            OrchestratorNodeHandle& handler,
            uint32_t domain = 0);

    /**
     * @brief Destroy the Orchestrator Node object
     *
     * @note The deletion of the handler is responsibility of the user.
     */
    ~OrchestratorNode();

    /**
     * @brief Get the task data from DB given the task_id and node identifier.
     * @param [in] task_id id identifier of the task that has new data available
     * @param [in] node_id id identifier of the node that triggered the new status
     * @param [in,out]  data pointer that will be redirected to the data
     * @return RetCode_t indicating the result of the operation
     */
    RetCode_t get_task_data(
            const types::TaskId& task_id,
            const NodeID& node_id,
            void*& data);

    /**
     * @brief Get the node status from DB given node identifier.
     * @param [in] node_id id identifier of the node that triggered the new status
     * @param [in,out]  status pointer that will be redirected to the status data
     * @return RetCode_t indicating the result of the operation
     */
    RetCode_t get_node_status(
            const NodeID& node_id,
            const types::NodeStatus*& status);

    /**
     * @brief This method reserves a new Task cache in the DB and returns the place
     * where to fill the UserInput entry structure.
     * @note It must be called before start_task()
     * @return A pair containing the TaskId and a pointer to the UserInput structure.
     */
    std::pair<types::TaskId, types::UserInput*> prepare_new_task();

    /**
     * @brief This method reserves a new Task cache in the DB and returns the place
     * where to fill the UserInput entry structure.
     * @param [in] task_id identifier of the previous task from which to iterate
     * @param [in] last_task_id identifier of the last task from the problem
     * @note It must be called before start_task()
     * @return A pair containing the TaskId and a pointer to the UserInput structure.
     */
    std::pair<types::TaskId, types::UserInput*> prepare_new_iteration(
            const types::TaskId& task_id,
            const types::TaskId& last_task_id);

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
     * @brief This method sends a request to node via service for changing its configuration and returns the response.
     * @param [in] req configuration request, contain which node and configuration file
     * @return The response type indicating the result of the configuration request.
     */
    types::ResponseType configuration_request(
            const types::RequestType& req);

    /**
     * @brief Public method to get the mutex in order to correctly synchronise user
     * handle calls.
     * @return A reference to the mutex.
     */
    inline std::mutex& get_mutex()
    {
        return mtx_;
    }

    /**
     * @brief Used to retrieve the associated OrchestratorNodeHandle.
     * @return A pointer to the OrchestratorNodeHandle.
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

    /**
     * @brief Handle to manage the node status and node output callbacks
     * @note The deletion of the handler is responsibility of the user.
     */
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

    std::condition_variable cv_;

    types::ResponseType res_;
    sustainml::core::RequestReplier* req_res_;

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

