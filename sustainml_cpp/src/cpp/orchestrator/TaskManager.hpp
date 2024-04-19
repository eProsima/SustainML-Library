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
 * @file TaskManager.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP

#include <atomic>
#include <mutex>

#include <common/Common.hpp>

namespace sustainml {
namespace orchestrator {

/**
 * @brief Class that ensures that unique global problem_id is created and updated.
 */
class TaskManager
{
public:

    TaskManager() = default;
    virtual ~TaskManager() = default;

    std::mutex mtx_;
    std::atomic<uint32_t> problem_id_;

    /**
     * @brief Create a new unique task_id. It only needs to account for the problem_id.
     */
    inline types::TaskId create_new_task_id()
    {
        types::TaskId task_id;
        problem_id_.fetch_add(1);
        task_id.problem_id(problem_id_.load());
        task_id.iteration_id(1);
        return task_id;
    }

    /**
     * @brief Sets the task id to the status of the system
     */
    inline void update_task_id(
            const types::TaskId& task_id)
    {
        std::lock_guard<std::mutex> _{mtx_};
        if (task_id.problem_id() > problem_id_.load())
        {
            problem_id_.exchange(task_id.problem_id());
        }
    }
};


} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP
