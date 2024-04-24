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
 * @file TaskDB.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP

#include <map>
#include <shared_mutex>
#include <tuple>

#include <core/Constants.hpp>
#include <types/types.h>

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
    bool insert_task_data_nts(
            const types::TaskId& task_id,
            const T& data);

    /**
     * @brief Retrieves data from the DB given the task name
     * @warning Non thread safe
     */
    template <typename T>
    bool get_task_data_nts(
            const types::TaskId& task_id,
            T*&);

    /**
     * @brief Allocates a new entry in the DB
     * @warning Non thread safe
     */
    bool prepare_new_entry_nts(
            const types::TaskId& task_id,
            bool is_new_iteration);

    /**
     * @brief Checks whether a given entry exists in the DB
     * @warning Non thread safe
     */
    bool entry_exists_nts(
            const types::TaskId& task_id);


    /**
     * @brief Checks whether a given entry exists in the DB
     * Non thread safe
     */
    inline std::mutex& get_mutex()
    {
        return mtx_;
    }

    /**
     * @brief Copies the data indicated in data_to_copy from one TaskId to another
     */
    bool copy_data_nts(
            const types::TaskId& source,
            const types::TaskId& dest,
            const std::vector<NodeID>& data_to_copy);

    friend std::ostream& operator <<(
            std::ostream& os,
            const TaskDB<Args...>& db)
    {
        for (auto& db_problem : db.db_)
        {
            for (auto& db_iteration : db_problem.second)
            {
                os << types::TaskId{db_problem.first, db_iteration.first} << " : [ ";
                os << std::get<types::AppRequirements>(db_iteration.second).task_id() << " ";
                os << std::get<types::CO2Footprint>(db_iteration.second).task_id() << " ";
                os << std::get<types::HWConstraints>(db_iteration.second).task_id() << " ";
                os << std::get<types::HWResource>(db_iteration.second).task_id() << " ";
                os << std::get<types::MLModelMetadata>(db_iteration.second).task_id() << " ";
                os << std::get<types::MLModel>(db_iteration.second).task_id() << " ";
                os << std::get<types::UserInput>(db_iteration.second).task_id() << " ";
                os << "]" << std::endl;
            }
        }
        return os;
    }

protected:

    std::mutex mtx_;
    std::map<uint32_t, std::map<uint32_t, std::tuple<Args...>>> db_;
};

template <typename ... Args>
TaskDB<Args...>::~TaskDB()
{
    std::lock_guard<std::mutex> lock(mtx_);
}

template <typename ... Args>
template <typename T>
bool TaskDB<Args...>::insert_task_data_nts(
        const types::TaskId& task_id,
        const T& data)
{
    bool ret_code = false;

    if (entry_exists_nts(task_id))
    {
        T& db_data = std::get<T>(db_[task_id.problem_id()][task_id.iteration_id()]);
        db_data = data;
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to insert a data element with an unknown task id" << task_id);
    }

    return ret_code;
}

template <typename ... Args>
template <typename T>
bool TaskDB<Args...>::get_task_data_nts(
        const types::TaskId& task_id,
        T*& data)
{
    bool ret_code = false;

    if (entry_exists_nts(task_id))
    {
        T& db_data = std::get<T>(db_[task_id.problem_id()][task_id.iteration_id()]);
        data = &db_data;
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to get a data element with an unknown task id" << task_id);
    }

    return ret_code;
}

template <typename ... Args>
bool TaskDB<Args...>::prepare_new_entry_nts(
        const types::TaskId& task_id,
        bool is_new_iteration)
{
    bool ret_code = false;

    if (!entry_exists_nts(task_id))
    {
        db_[task_id.problem_id()][task_id.iteration_id()];
        ret_code = true;
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to prepare an already existing entry " << task_id);
    }

    return ret_code;
}

template <typename ... Args>
bool TaskDB<Args...>::entry_exists_nts(
        const types::TaskId& task_id)
{
    bool ret_code = false;

    auto it_problem_id = db_.find(task_id.problem_id());

    if (it_problem_id != db_.end())
    {
        auto it_id = db_[task_id.problem_id()].find(task_id.iteration_id());
        if (it_id != db_[task_id.problem_id()].end())
        {
            ret_code = true;
        }
    }

    return ret_code;
}

template<typename T>
void substitute_data(
        const T& old_data,
        T& new_data)
{
    new_data = old_data;
    new_data.task_id(old_data.task_id());
}

template <typename ... Args>
bool TaskDB<Args...>::copy_data_nts(
        const types::TaskId& source,
        const types::TaskId& dest,
        const std::vector<NodeID>& data_to_copy)
{
    bool ret_code = false;

    auto it_source_problem_id = db_.find(source.problem_id());

    if (it_source_problem_id != db_.end())
    {
        auto it_source_iteration_id = db_[source.problem_id()].find(source.iteration_id());
        if (it_source_iteration_id != db_[source.problem_id()].end())
        {
            // check that the destination exists
            auto it_dest_problem_id = db_.find(dest.problem_id());
            if (it_dest_problem_id != db_.end())
            {
                auto it_dest_iteration_id = db_[dest.problem_id()].find(dest.iteration_id());
                if (it_dest_iteration_id != db_[dest.problem_id()].end())
                {
                    // copy the data
                    for (size_t i = 0; i < data_to_copy.size(); i++)
                    {
                        switch (data_to_copy[i])
                        {
                            case NodeID::ID_APP_REQUIREMENTS:
                            {
                                types::AppRequirements& source_db_data = std::get<types::AppRequirements>(
                                    it_source_iteration_id->second);
                                types::AppRequirements& dest_db_data = std::get<types::AppRequirements>(
                                    it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_CARBON_FOOTPRINT:
                            {
                                types::CO2Footprint& source_db_data = std::get<types::CO2Footprint>(
                                    it_source_iteration_id->second);
                                types::CO2Footprint& dest_db_data = std::get<types::CO2Footprint>(
                                    it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_HW_CONSTRAINTS:
                            {
                                types::HWConstraints& source_db_data = std::get<types::HWConstraints>(
                                    it_source_iteration_id->second);
                                types::HWConstraints& dest_db_data = std::get<types::HWConstraints>(
                                    it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_HW_RESOURCES:
                            {
                                types::HWResource& source_db_data = std::get<types::HWResource>(
                                    it_source_iteration_id->second);
                                types::HWResource& dest_db_data = std::get<types::HWResource>(
                                    it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_ML_MODEL_METADATA:
                            {
                                types::MLModelMetadata& source_db_data = std::get<types::MLModelMetadata>(
                                    it_source_iteration_id->second);
                                types::MLModelMetadata& dest_db_data = std::get<types::MLModelMetadata>(
                                    it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_ML_MODEL:
                            {
                                types::MLModel& source_db_data =
                                        std::get<types::MLModel>(it_source_iteration_id->second);
                                types::MLModel& dest_db_data = std::get<types::MLModel>(it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::ID_ORCHESTRATOR:
                            {
                                types::UserInput& source_db_data = std::get<types::UserInput>(
                                    it_source_iteration_id->second);
                                types::UserInput& dest_db_data =
                                        std::get<types::UserInput>(it_dest_iteration_id->second);
                                substitute_data(source_db_data, dest_db_data);
                                ret_code = true;
                                break;
                            }
                            case NodeID::MAX:
                            case NodeID::UNKNOWN:
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB,
                            "Trying to copy data to an unknown destination iteration id " << dest);
                }
            }
            else
            {
                EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB,
                        "Trying to copy data to an unknown destination problem id " << dest);
            }

        }
        else
        {
            EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to copy data from an unknown source iteration id " << source);
        }
    }
    else
    {
        EPROSIMA_LOG_ERROR(ORCHESTRATOR_DB, "Trying to copy data from an invalid source " << source);
    }

    return ret_code;
}

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_TASKDB_HPP
