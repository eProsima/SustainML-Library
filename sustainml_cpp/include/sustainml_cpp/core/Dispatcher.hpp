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

    class Dispatcher
    {

    public:

        Dispatcher(Node *node);

        ~Dispatcher();

        void start();

        void stop();

        void register_retriever(SamplesQueryable* sr);

        void notify(const int &task_id);

    private:

        void process(const int& task_id);

        void routine();

        eprosima::utils::SlotThreadPool thread_pool_;

        std::condition_variable pool_cv;

        std::mutex mtx_, taskid_mtx_;

        std::function<void()> slot_;

        Node* node_;

        std::queue<int> taskid_buffer_;

        // collection of <taskid, number_times>
        // No task can be received twice
        std::map<int, int> taskid_tracker_;

        std::vector<SamplesQueryable*> sample_queryables_;

        std::atomic<bool> stop_;
    };

}

#endif // SUSTAINMLCPP_CORE_DISPATCHER_HPP
