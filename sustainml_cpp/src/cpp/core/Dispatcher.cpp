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

#include <sustainml_cpp/core/Dispatcher.hpp>

namespace sustainml {

    Dispatcher::Dispatcher(Node *node) :
        node_(node),
        stop_(false),
        thread_pool_(N_THREADS_DEFAULT)
    {
        //! TODO
    }

    Dispatcher::~Dispatcher()
    {
        //! TODO
    }

    void Dispatcher::start()
    {
        //! TODO
    }

    void Dispatcher::stop()
    {
        //! TODO
    }

    void Dispatcher::register_retriever(SamplesQueryable* sr)
    {
        //! TODO
    }

    void Dispatcher::notify(const int &task_id)
    {
        //! TODO
    }

    void Dispatcher::process(const int& task_id)
    {
        //! TODO
    }

    void Dispatcher::routine()
    {
        //! TODO
    }

} // namespace sustainml
