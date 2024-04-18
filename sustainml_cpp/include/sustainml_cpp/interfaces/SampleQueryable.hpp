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
 * @file SampleQueryable.hpp
 */

#ifndef SUSTAINMLCPP_INTERFACES_SAMPLEQUERYABLE_HPP
#define SUSTAINMLCPP_INTERFACES_SAMPLEQUERYABLE_HPP

#include <vector>

#include <types/types.h>

namespace sustainml {
namespace interfaces {

    struct SampleQueryable
    {
        /**
        * @brief Retrieves a void-casted pointer of a sample by id.
        *
        * @param id task_id key of the sample.
        *
        * @return pointer to the sample
        */
        virtual void* retrieve_sample_from_taskid(const types::TaskId &id) = 0;

        /**
        * @brief Retrieves the id of the sample queryable.
        *
        * @return Id of the queryable
        */
        virtual const int& get_id() = 0;

    };

} // namespace interfaces
} // namespace sustainml

#endif // SUSTAINMLCPP_SAMPLESQUERYABLE_HPP
