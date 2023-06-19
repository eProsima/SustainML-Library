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
 * @file SamplesQueue.hpp
 */


#ifndef SUSTAINMLCPP_CORE_SAMPLESQUEUEHPP
#define SUSTAINMLCPP_CORE_SAMPLESQUEUEHPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/interfaces/SampleQueryable.hpp>

#include <map>
#include <mutex>
#include <memory>

namespace sustainml {

    class Node;

    /**
    * @brief Queue implementation for storing samples.
    * Samplesa are stored in a map indexed by the task_id.
    */
    template <typename T>
    class SamplesQueue : public SampleQueryable<T>
    {

    public:

        SamplesQueue(
            Node* node);

        ~SamplesQueue();

        /**
        * @brief Inserts an element into the queue.
        *
        * Thread safe operation.
        *
        * @param elem element to insert.
        */
        void insert_element(const std::shared_ptr<T> &elem);

        /**
        * @brief Remove an element from the queue.
        *
        * Thread safe operation.
        *
        * @param id Sample key to remove.
        */
        void remove_element_by_taskid(const int& id);

        /**
        * @brief Retrieves a type-erased pointer of a sample by id.
        * Implements the SampleQueryable interface
        *
        * @param id task_id key of the sample.
        */
        void* retrieve_sample_from_taskid(const int &id) override;

        SamplesQueue<T>* get_queue();

    private:

        Node* node_;

        std::map<int, std::shared_ptr<T>> queue_;

        std::mutex mtx_;

        const std::string name{std::string(typeid(T).name())};

    };

}

#endif // SUSTAINMLCPP_CORE_SAMPLESQUEUEHPP
