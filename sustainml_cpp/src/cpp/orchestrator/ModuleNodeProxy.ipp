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
 * @file ModuleNodeProxy.ipp
 */


#include <orchestrator/ModuleNodeProxy.hpp>
#include <orchestrator/TaskManager.hpp>

#include "TaskDB.ipp"

#include <common/Common.hpp>

namespace sustainml {
namespace orchestrator {

template<typename T>
void ModuleNodeProxy::publish_data_for_iteration_(
        const types::TaskId& task_id,
        T* iter_data)
{
    types::TaskId task_id_to_take_from_db(task_id);

    // If it is a baseline, send the previous iteration data in baselines except from the first one
    // which is always 1,1
    if (publish_baseline_ && task_id.iteration_id() > 1)
    {
        task_id_to_take_from_db.iteration_id(task_id_to_take_from_db.iteration_id() - 1);
    }

    std::unique_lock<std::mutex> lock(task_db_->get_mutex());
    if (task_db_->get_task_data_nts(task_id_to_take_from_db, iter_data))
    {
        lock.unlock();
        if (nullptr != iter_data)
        {
            // Publish the baseline with the new iteration id
            // If it not a baseline, the iteration id it is already the correct one
            iter_data->task_id(task_id);

            baseline_writer_->write(iter_data->get_impl());

            // Restore the original iteration id
            iter_data->task_id(task_id_to_take_from_db);
        }
        else
        {
            EPROSIMA_LOG_ERROR(MODULE_PROXY, "Error publishing data for iteration");
        }

    }
}

} // namespace orchestrator
} // namespace sustainml
