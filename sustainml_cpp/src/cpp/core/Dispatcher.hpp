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
 * @file Dispatcher.hpp
 */

#ifndef SUSTAINMLCPP_CORE_DISPATCHER_HPP
#define SUSTAINMLCPP_CORE_DISPATCHER_HPP

#include <sustainml_cpp/interfaces/SampleQueryable.hpp>

#include <functional>
#include <iostream>
#include <mutex>
#include <memory>
#include <thread>
#include <map>
#include <condition_variable>
#include <vector>
#include <queue>
#include <atomic>

#include <cpp_utils/thread_pool/pool/SlotThreadPool.hpp>

namespace sustainml {
namespace core {

    constexpr int N_THREADS_DEFAULT = 2;
    constexpr int INITIAL_N_QUEUES = 3;
    constexpr eprosima::utils::TaskId DISPATCHER_ROUTINE_ID = 1;

    class Node;
    class SampleQueryable;

    /**
    * @brief This class tracks the number of times a sample of a particular
    * task_id has been received in different queues. When a task_id has
    * received the expected number of samples, is in charge of retrieving
    * the samples so that the Node can invoke the user callback.
    *
    * It is served by a Thread Pool that simultaneously executes the routine()
    * when a new task_id is received.
    */
    class Dispatcher
    {

        friend class Node;

    public:

        Dispatcher(Node *node);

        ~Dispatcher();

        /**
        * @brief Starts spinning the thread pool.
        */
        void start();

        /**
        * @brief Returns whether the Dispatcher has been started.
        */
        bool is_active();

        /**
        * @brief Stops the Dispatcher execution.
        */
        void stop();

        /**
        * @brief Register a new Queue from which to take samples. The expected
        * number of samples for a task_id is the number of SampleQueryable registered.
        *
        * @param sr Interface from which to retrieve the samples of a particular task_id.
        */
        void register_sample_queryable(interfaces::SampleQueryable* sr);

        /**
        * @brief Method used to notify the Dispatcher that a new sample for that task_id
        * has been received.
        *
        * @param task_id Task identifier
        */
        void notify(const types::TaskId & task_id);

    private:

        /**
        * @brief Implements the main logic of the Dispatcher.
        * Increments the counter for that task_id and, if all the samples are received, it
        * retrieves the samples from the queues and invokes the user callback.
        *
        * @param task_id Task identifier
        */
        void process(const types::TaskId & task_id);

        /**
        * @brief Function that each thread executes. In a thread-safe fashion, pops
        * a task_id from the taskid_buffer and processes it.
        */
        void routine();

        eprosima::utils::SlotThreadPool thread_pool_;

        std::condition_variable pool_cv;

        std::mutex mtx_, taskid_mtx_;

        std::function<void()> slot_;

        Node* node_;

        std::queue<types::TaskId> taskid_buffer_;

        // collection of <taskid, std::vector<queue_id>>
        // Current implementation assumes that no task_id
        // can be received twice in a queue
        // but it can be easily added by also tracking the
        // queue_id
        std::map<types::TaskId, int> taskid_tracker_;

        std::vector<interfaces::SampleQueryable*> sample_queryables_;

        std::atomic<bool> stop_;

        std::atomic<bool> started_;
    };

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_DISPATCHER_HPP
