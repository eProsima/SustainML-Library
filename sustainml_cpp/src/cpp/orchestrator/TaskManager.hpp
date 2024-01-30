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
 * @file TaskManager.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP

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
    virtual ~TaskManager();

    /**
    * @brief Create a new unique task_id.
    */
    int create_new_task_id()
    {
        static int task_id = common::INVALID_ID;
        ++task_id;
        return task_id;
    }

};


} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKMANAGER_HPP
