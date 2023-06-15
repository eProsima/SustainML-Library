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

    constexpr int N_THREADS_DEFAULT = 2;

    class Node;
    class SamplesQueryable;

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

    public:

        Dispatcher(Node *node);

        ~Dispatcher();

        /**
        * @brief Starts spinning the thread pool.
        */
        void start();

        /**
        * @brief Stops the Dispatcher execution.
        */
        void stop();

        /**
        * @brief Register a new Queue from which to take samples. The expected
        * number of samples for a task_id is the number of SamplesQueryable registered.
        *
        * @param sr Interface from which to retrieve the samples of a particular task_id.
        */
        void register_retriever(SamplesQueryable* sr);

        /**
        * @brief Method used to notify the Dispatcher that a new sample for that task_id
        * has been received.
        *
        * @param task_id Task identifier
        */
        void notify(const int &task_id);

    private:

        /**
        * @brief Implements the main logic of the Dispatcher.
        * Increments the counter for that task_id and, if all the samples are received, it
        * retrieves the samples from the queues and invokes the user callback.
        *
        * @param task_id Task identifier
        */
        void process(const int& task_id);

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

        std::queue<int> taskid_buffer_;

        // collection of <taskid, number_times>
        // No task_id can be received twice in a queue
        std::map<int, int> taskid_tracker_;

        std::vector<SamplesQueryable*> sample_queryables_;

        std::atomic<bool> stop_;
    };

}

#endif // SUSTAINMLCPP_CORE_DISPATCHER_HPP
