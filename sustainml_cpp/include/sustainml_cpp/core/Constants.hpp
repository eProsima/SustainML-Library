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
 * @file Constants.hpp
 */

#ifndef SUSTAINMLCPP_CORE_CONSTANTS_HPP
#define SUSTAINMLCPP_CORE_CONSTANTS_HPP

#include <cstdint>

namespace sustainml {

/**
 * @brief Enum class that defines a series of node identifiers.
 */
enum class NodeID
{
    ID_TASK_ENCODER,
    ID_MACHINE_LEARNING,
    ID_HARDWARE_RESOURCES,
    ID_CARBON_FOOTPRINT,
    // Proxies end here since the MAX
    // is used to allocate proxy resources
    // in the orchestrator
    MAX,
    ID_ORCHESTRATOR,
    UNKNOWN
};

/**
 * @brief Class used to return different return code statuses.
 */
class RetCode_t
{
    uint32_t value_;

public:

    enum ReturnCodeValue
    {
        RETCODE_OK = 0,
        RETCODE_ERROR,
        RETCODE_UNSUPPORTED,
        RETCODE_TIMEOUT,
        RETCODE_NO_DATA
    };

    RetCode_t()
        : value_(RETCODE_OK)
    {
    }

    RetCode_t(
            uint32_t e)
    {
        value_ = e;
    }

    bool operator ==(
            const RetCode_t& c) const
    {
        return value_ == c.value_;
    }

    bool operator !=(
            const RetCode_t& c) const
    {
        return value_ != c.value_;
    }

    explicit operator bool() = delete;

    uint32_t operator ()() const
    {
        return value_;
    }

    bool operator !() const
    {
        return value_ != 0;
    }

};

inline bool operator ==(
        RetCode_t::ReturnCodeValue a,
        const RetCode_t& b)
{
    return b.operator ==(
        a);
}

inline bool operator !=(
        RetCode_t::ReturnCodeValue a,
        const RetCode_t& b)
{
    return b.operator !=(
        a);
}

inline bool operator ==(
        uint32_t a,
        const RetCode_t& b)
{
    return b.operator ==(
        a);
}

inline bool operator !=(
        uint32_t a,
        const RetCode_t& b)
{
    return b.operator !=(
        a);
}

} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_CONSTANTS_HPP
