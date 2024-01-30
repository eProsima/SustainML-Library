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

namespace sustainml {
namespace orchestrator {

/**
* @brief Class that represents the DataBase
*/
template <typename ...Args>
class TaskDB
{
public:
    TaskDB() = default;
    virtual ~TaskDB();

    /**
    * @brief Inserts new data in the DB.
    */
    template <typename T>
    bool insert_task_data(const int& task_id, const T& data);

    /**
    * @brief Retrieves data from the DB given the task name
    */
    template <typename T>
    T& get_task_data(const int &task_id);

protected:

    std::mutex mtx_;
    std::map<int, std::tuple<Args...>> db_;

};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP
