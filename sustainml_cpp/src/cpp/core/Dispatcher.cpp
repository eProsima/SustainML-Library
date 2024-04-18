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
 * @file Dispatcher.cpp
 */

#include <core/Dispatcher.hpp>

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/interfaces/SampleQueryable.hpp>

#include <common/Common.hpp>

namespace sustainml {
namespace core {

Dispatcher::Dispatcher(
        Node* node)
    : thread_pool_(N_THREADS_DEFAULT)
    , node_(node)
    , stop_(false)
    , started_(false)
{
    sample_queryables_.reserve(INITIAL_N_QUEUES);
}

Dispatcher::~Dispatcher()
{
    stop();
}

bool Dispatcher::is_active()
{
    return started_.load(std::memory_order_relaxed);
}

void Dispatcher::start()
{
    if (!started_)
    {
        thread_pool_.enable();

        // Feed the routine in thread pool
        thread_pool_.slot(
            DISPATCHER_ROUTINE_ID,
            std::bind(&Dispatcher::routine, this));

        started_.store(true);
    }
}

void Dispatcher::stop()
{
    thread_pool_.disable();
}

void Dispatcher::register_sample_queryable(
        interfaces::SampleQueryable* sr)
{
    sample_queryables_.emplace_back(sr);
}

void Dispatcher::notify(
        const types::TaskId& task_id)
{
    if (started_.load(std::memory_order_relaxed))
    {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            taskid_buffer_.push(task_id);
        }

        thread_pool_.emit(DISPATCHER_ROUTINE_ID);
    }
    else
    {
        EPROSIMA_LOG_ERROR(DISPATCHER,
                node_->name() << " Dispatcher discarding sample with task_id " << task_id <<
                ", not initialized");
    }
}

void Dispatcher::process(
        const types::TaskId& task_id)
{
    int expected_hits = sample_queryables_.size();

    bool all_received = false;

    {
        std::lock_guard<std::mutex> lock(taskid_mtx_);
        auto it = taskid_tracker_.find(task_id);

        if (it == taskid_tracker_.end())
        {
            EPROSIMA_LOG_INFO(DISPATCHER, node_->name() << " Initializing task_id " << task_id);
            taskid_tracker_[task_id] = 1;
        }
        else
        {
            it->second += 1;
            EPROSIMA_LOG_INFO(DISPATCHER,
                    node_->name() << " Taskid_tracker_[task_id] " << task_id << " n_times " << it->second);
        }

        if (taskid_tracker_[task_id] == expected_hits)
        {
            all_received = true;
            taskid_tracker_.erase(task_id);
        }
    }

    if (all_received)
    {
        std::vector<std::pair<int, void*>> samples;
        samples.reserve(sample_queryables_.size());

        for (auto& sq : sample_queryables_)
        {
            void* sample = sq->retrieve_sample_from_taskid(task_id);

            if (nullptr != sample)
            {
                samples.push_back(std::make_pair(sq->get_id(), sample));
            }
            else
            {
                return;
            }
        }
        node_->publish_to_user(task_id, samples);
    }
}

void Dispatcher::routine()
{
    types::TaskId task_id;

    {
        std::unique_lock<std::mutex> lock(mtx_);
        task_id = taskid_buffer_.front();
        taskid_buffer_.pop();
    }

    if (task_id.data_id() != common::INVALID_ID)
    {
        process(task_id);
    }
    else
    {
        EPROSIMA_LOG_ERROR(DISPATCHER, "Invalid Task Id in queue");
    }

}

} // namespace core
} // namespace sustainml
