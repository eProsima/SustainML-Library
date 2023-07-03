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
 * @file SamplesQueue.cpp
 */

#include <sustainml_cpp/core/SamplesQueue.hpp>

#include <common/Common.hpp>
#include <utils/SamplePool.hpp>

namespace sustainml {
namespace core {

    template <typename T>
    SamplesQueue<T>::SamplesQueue(
            Node* node) :
            queue_id(common::queue_name_to_id(std::string(typeid(T).name()))),
            node_(node),
            pool_(new sustainml::utils::SamplePool<T>())
    {
        auto dispatcher = node_->get_dispatcher();

        if (auto disp_p = dispatcher.lock())
        {
            disp_p->register_sample_queryable(this);
        }
        else
        {
            EPROSIMA_LOG_ERROR(SAMPLES_QUEUE, "Could not initialize Samples Queue " << queue_id);
        }
    }

    template <typename T>
    SamplesQueue<T>::~SamplesQueue()
    {
        //! TODO
    }

    template <typename T>
    T* SamplesQueue<T>::get_new_cache()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return pool_->get_new_cache_nts();
    }

    template <typename T>
    void SamplesQueue<T>::release_cache(T* cache)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        pool_->release_cache_nts(cache);
    }

    template <typename T>
    void SamplesQueue<T>::insert_element(T* &elem)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.insert(std::make_pair(elem->task_id(), std::make_pair(elem, false)));
    }

    template <typename T>
    void SamplesQueue<T>::remove_element_by_taskid(const int& id)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        pool_->release_cache_nts(queue_[id].first);
        queue_.erase(id);
    }

    template <typename T>
    void* SamplesQueue<T>::retrieve_sample_from_taskid(const int &id)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        T* sample {nullptr};

        auto it = queue_.find(id);

        if (it != queue_.end())
        {
            if (!it->second.second)
            {
                sample = it->second.first;
                it->second.second = true;
            }
            else
            {
                EPROSIMA_LOG_ERROR(SAMPLES_QUEUE, "Trying to retrieve an already taken sample");
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(SAMPLES_QUEUE, "Trying to retrieve an invalid sample for " << id << " in " << queue_id);
        }

        return static_cast<void*>(sample);
    }

    template <typename T>
    SamplesQueue<T>* SamplesQueue<T>::get_queue()
    {
        return this;
    }

    template <typename T>
    const int& SamplesQueue<T>::get_id()
    {
        return queue_id;
    }

} // namespace core
} // namespace sustainml
