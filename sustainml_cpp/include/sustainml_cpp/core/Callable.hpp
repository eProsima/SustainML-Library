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
 * @file Callable.hpp
 */

#ifndef SUSTAINMLCPP_CORE_CALLABLE_HPP
#define SUSTAINMLCPP_CORE_CALLABLE_HPP

#include <functional>

namespace sustainml {

    template <typename ...Args>
    class Callable
    {
    public:
        void register_cb(std::function<void(Args...)> fn)
        {
            user_callback_ = fn;
        }

        std::function<void(Args...)> user_callback_;
    };

}

#endif // SUSTAINMLCPP_CORE_CALLABLE_HPP
