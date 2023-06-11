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


#ifndef SUSTAINML_CPP__SAMPLESQUEUEHPP
#define SUSTAINML_CPP__SAMPLESQUEUEHPP

#include <sustainml_cpp/core/Node.hpp>
#include <sustainml_cpp/interfaces/SampleQueryable.hpp>

#include <map>
#include <mutex>
#include <memory>

namespace sustainml {

    class Node;

    template <typename T>
    class SamplesQueue : public SampleQueryable<T>
    {

    public:

        SamplesQueue(
            Node* node);

        ~SamplesQueue();

        void insert_element(const std::shared_ptr<T> &elem);

        void remove_element_by_taskid(const int& id);

        std::shared_ptr<T> get_input_sample_from_taskid(const int &id);

    private:

        Node* node_;

        std::map<int, std::shared_ptr<T>> queue_;

        std::mutex mtx_;

        const std::string name{std::string(typeid(T).name())};

    };

}

#endif // SUSTAINML_CPP__SAMPLESQUEUEHPP
