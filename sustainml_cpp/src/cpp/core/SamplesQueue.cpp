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

namespace sustainml {

    template <typename T>
    SamplesQueue<T>::SamplesQueue(
            Node* node) :
            node_(node)
    {
        //! TODO
    }

    template <typename T>
    SamplesQueue<T>::~SamplesQueue()
    {
        //! TODO
    }

    template <typename T>
    void SamplesQueue<T>::insert_element(const std::shared_ptr<T> &elem)
    {
        //! TODO
    }

    template <typename T>
    void SamplesQueue<T>::remove_element_by_taskid(const int& id)
    {
        //! TODO
    }

    template <typename T>
    void* SamplesQueue<T>::retrieve_sample_from_taskid(const int &id)
    {
        //! TODO
        return nullptr;
    }

    template <typename T>
    SamplesQueue<T>* SamplesQueue<T>::get_queue()
    {
        return this;
    }

} // namespace sustainml
