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
 * @file OrchestratorNode.hpp
 */


#ifndef SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP
#define SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP

#include <memory>
#include <mutex>

#include <sustainml_cpp/core/Constants.hpp>
#include <sustainml_cpp/types/types.h>

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
template <typename ...Args> class TaskDB;
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
    */
    virtual void on_new_node_output(const NodeID &id, void* data) = 0;

    /**
    * @brief Callback to notify the user that the status of a node has changed.
    */
    virtual void on_node_status_change(const NodeID &id, const types::NodeStatus& status) = 0;
};

class OrchestratorNode
{
public:

    using TaskDB_t =  TaskDB<
                        types::UserInput,
                        types::EncodedTask,
                        types::MLModel,
                        types::HWResource,
                        types::CO2Footprint>;

    OrchestratorNode(OrchestratorNodeHandle& handler);

    /**
    * @brief Get the task data from DB given the task_id and node identifier.
    */
    RetCode_t get_task_data(const int &task_id, const NodeID &node_id, void* data);

    /**
    * @brief Get the node status from DB given node identifier.
    */
    RetCode_t get_node_status(const NodeID &node_id, const types::NodeStatus &status);

    /**
    * @brief This method reserves a new Task cache in the DB and returns the place
    * where to fill the UserInput entry structure.
    * @note It must be called before start_task()
    */
    std::pair<int, types::UserInput*> prepare_new_task();

    /**
    * @brief This method triggers a new task with a previously prepared task_id and
    * a pointer to the UserInput data structure.
    */
    bool start_task(const int &task_id, types::UserInput* ui);

    /**
    * @brief This method sends the specified Node Control command to the related node.
    */
    void send_control_command(const types::NodeControl& cmd);

    /**
    * @brief Public method to get the mutex in order to correctly synchronise user
    * handle calls.
    */
    inline std::recursive_mutex& get_mutex()
    {
        return mtx_;
    }

    /**
    * @brief Used to retrieve the associated OrchestratorNodeHandle.
    */
    const OrchestratorNodeHandle& get_handler();

protected:

    /**
    * @brief Used for intializing the Orchestrator
    */
    void init();

    OrchestratorNodeHandle& handler_;

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Topic* control_topic_;

    eprosima::fastdds::dds::Publisher* pub_;

    eprosima::fastdds::dds::DataWriter* control_writer_;

    std::array<std::unique_ptr<ModuleNodeProxy>, (size_t)NodeID::MAX> node_proxies_;

    std::shared_ptr<TaskDB_t> task_db_;

    std::unique_ptr<TaskManager> task_man_;

    std::recursive_mutex mtx_;

};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_ORCHESTRATOR_ORCHESTRATORNODE_HPP

