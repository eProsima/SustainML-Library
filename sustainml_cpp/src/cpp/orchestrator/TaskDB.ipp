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
 * @file TaskDB.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP

#include <map>
#include <shared_mutex>
#include <tuple>

#include <fastrtps/log/Log.h>

namespace sustainml {
namespace orchestrator {

/**
 * @brief Class that represents the DataBase
 */
template <typename ... Args>
class TaskDB
{
public:

    TaskDB() = default;
    virtual ~TaskDB();

    /**
     * @brief Inserts new data in the DB.
     */
    template <typename T>
    bool insert_task_data(
            const int& task_id,
            const T& data);

    /**
     * @brief Retrieves data from the DB given the task name
     */
    template <typename T>
    bool get_task_data(
            const int& task_id,
            T*&);

    /**
     * @brief Allocates a new entry in the DB
     */
    bool prepare_new_entry(
            const int& task_id);

protected:

    std::mutex mtx_;
    std::map<int, std::tuple<Args...>> db_;

};

template <typename ... Args>
TaskDB<Args...>::~TaskDB()
{
    std::lock_guard<std::mutex> lock(mtx_);
}

template <typename ... Args>
template <typename T>
bool TaskDB<Args...>::insert_task_data(
        const int& task_id,
        const T& data)
{
    bool ret_code = false;

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = db_.find(task_id);

    if (it != db_.end())
    {
        T& db_data = std::get<T>(it->second);
        db_data = data;
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Inserting a data element with an unknown task_id");
    }

    return ret_code;
}

template <typename ... Args>
template <typename T>
bool TaskDB<Args...>::get_task_data(
        const int& task_id,
        T*& data)
{
    bool ret_code = false;

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = db_.find(task_id);

    if (it != db_.end())
    {
        T& db_data = std::get<T>(it->second);
        data = &db_data;
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to get a data element with an unknown task_id");
    }

    return ret_code;
}

template <typename ... Args>
bool TaskDB<Args...>::prepare_new_entry(
        const int& task_id)
{
    bool ret_code = false;

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = db_.find(task_id);

    if (it == db_.end())
    {
        db_[task_id];
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to reserve an already existing task id entry");
    }

    return ret_code;
}

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP
