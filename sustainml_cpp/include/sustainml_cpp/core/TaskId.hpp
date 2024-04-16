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
 * @file TaskId.hpp
 */

#ifndef SUSTAINMLCPP_CORE_TASKID_HPP
#define SUSTAINMLCPP_CORE_TASKID_HPP

#include <sustainml_cpp/config/Macros.hpp>
#include <sustainml_cpp/types/types.h>

namespace sustainml {

/**
 * @brief
 */
class TaskId
{
public:

    const char* UNDEFINED_TASK_NAME = "UNDEFINED_TASK_NAME";

    SUSTAINML_CPP_DLL_API TaskId(
            const std::string& name,
            const int& id)
            : id_(id)
            , iteration_(0)
            , name_(name)
    {

    }

    SUSTAINML_CPP_DLL_API TaskId(
            const int& id)
            : id_(id)
            , name_(UNDEFINED_TASK_NAME)
    {

    }

    SUSTAINML_CPP_DLL_API virtual ~TaskId() = default;

    SUSTAINML_CPP_DLL_API const std::string& name() const
    {
        return name_;
    }

    SUSTAINML_CPP_DLL_API void name(const std::string& name)
    {
        name_ = name;
    }

    SUSTAINML_CPP_DLL_API const int& iteration() const
    {
        return iteration_;
    }

    SUSTAINML_CPP_DLL_API void iteration(const int& iteration)
    {
        iteration_ = iteration;
    }

    SUSTAINML_CPP_DLL_API const int& id() const
    {
        return id_;
    }

private:

    int id_;
    int iteration_;
    std::string name_;
};

} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_NODE_HPP
