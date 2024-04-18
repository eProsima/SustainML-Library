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
 * @brief Class that manages the task ids.
 */
class TaskManager
{
public:

    TaskManager() = default;
    virtual ~TaskManager() = default;

    std::mutex mtx_;
    types::TaskId task_id_;

    /**
     * @brief Create a new unique task_id.
     */
    inline types::TaskId create_new_task_id()
    {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            task_id_.problem_id(task_id_.problem_id() + 1);
            task_id_.data_id(task_id_.data_id() + 1);
        }

        return task_id_;
    }

    /**
     * @brief Sets the task id to the status of the system
     */
    inline void update_task_id(
            const types::TaskId& task_id)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (task_id.problem_id() > task_id_.problem_id())
        {
            task_id_.problem_id() = task_id.problem_id();
        }

        if (task_id.data_id() > task_id_.data_id())
        {
            task_id_.data_id() = task_id.data_id();
        }
    }

};


} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP
