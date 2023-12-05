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
#include <iostream>
#include <mutex>
#include <tuple>

#include <sustainml_cpp/config/Macros.hpp>
#include <sustainml_cpp/types/types.h>

#include <fastrtps/log/Log.h>

namespace sustainml {
namespace core {
namespace helper {

template<typename T>
void see_type(
        T)
{
    EPROSIMA_LOG_INFO(CALLABLE, __PRETTY_FUNCTION__);
}

#ifndef SWIG_WRAPPER
//! Expand the parameter pack in runtime
//! It is mainly used before invoking user callback
template <std::size_t... Ts>
struct index {};

template <std::size_t N, std::size_t... Ts>
struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};

template <std::size_t... Ts>
struct gen_seq<0, Ts...> : index<Ts...> {};
#endif //SWIG_WRAPPER

} // namespace helper

/**
 * @brief This class is used for registering an input callback
 * with a variable number of arguments and generic types.
 *
 * It is meant to be inherited by the actual SustainML nodes.
 */
template <typename ... _TYPES>
class Callable
{

#ifndef SWIG_WRAPPER
    using tuple = std::tuple<_TYPES*...>;

    template <size_t N>
    using get = typename std::tuple_element<N, tuple>::type;

public:

    //! number of types in pack.
    static constexpr auto size = sizeof...(_TYPES);

    /**
     * @brief User callback
     */
    virtual void on_new_task_available(
            _TYPES& ... fn) = 0;

    /**
     * @brief Invokes the user callback with the arguments stored in user_cb_args_
     */
    template <std::size_t... Is>
    void invoke_user_cb(
            int task_id, helper::index<Is...>)
    {
        tuple* args;
        {
            std::lock_guard<std::mutex> lock (mtx_);
            args = &user_cb_args_[task_id];
        }
        on_new_task_available(*std::get<Is>(*args)...);
    }

    /**
     * @brief Returns the arguments with which the user callback
     * will be later invoked
     *
     * @return Reference to the user callback args (simple pointers)
     */
    tuple& create_and_get_user_cb_args(const int& task_id)
    {
        std::lock_guard<std::mutex> lock (mtx_);
        user_cb_args_.insert({task_id, tuple()});
        return user_cb_args_[task_id];
    }

    /**
     * @brief Erases the element from the map by key
     *
     */
    void remove_task_args(const int& task_id)
    {
        std::lock_guard<std::mutex> lock (mtx_);
        user_cb_args_.erase(task_id);
    }

private:

    std::mutex mtx_;
    std::map<int, tuple> user_cb_args_;
#else

public:

    /**
     * @brief user's callback to be implemented by the user
     */
    SUSTAINML_CPP_DLL_API virtual void on_new_task_available(
            _TYPES& ... fn) = 0;

#endif //SWIG_WRAPPER

};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_CALLABLE_HPP
