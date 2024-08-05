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
 * @file SamplePool.hpp
 */

#ifndef SUSTAINMLCPP_UTILS_SAMPLEPOOL_HPP
#define SUSTAINMLCPP_UTILS_SAMPLEPOOL_HPP

#include <fastrtps/log/Log.h>
#include <core/Options.hpp>

#include <memory>
#include <vector>

#include <iostream>

namespace sustainml {
namespace utils {
/*!
 *  @brief Helper class that stores samples until they are processed.
 *
 *  @warning Non thread safe
 */
template <typename T>
class SamplePool
{

public:

    explicit SamplePool(
            const core::Options& opts = core::Options())
        : caches_(new T[opts.sample_pool_size])
    {
        free_caches_.reserve(opts.sample_pool_size);

        for ( std::size_t i = 0; i < opts.sample_pool_size; ++i )
        {
            free_caches_.push_back( &caches_[i] );
        }
    }

    ~SamplePool()
    {
        delete [] caches_;
    }

    T* get_new_cache_nts()
    {
        T* cache = nullptr;

        if (!free_caches_.empty())
        {
            cache = free_caches_.back();
            free_caches_.pop_back();
        }

        return cache;
    }

    void release_cache_nts(
            T* cache)
    {
        if (nullptr != cache)
        {
            cache->reset();
            free_caches_.push_back(cache);
        }
        else
        {
            EPROSIMA_LOG_ERROR(SAMPLE_POOL, "Trying to release a null cache");
        }
    }

private:

    std::vector<T*> free_caches_;
    T* caches_;

};

} // namespace utils
} // namespace sustainml

#endif // SUSTAINMLCPP_UTILS_SAMPLEPOOL_HPP
