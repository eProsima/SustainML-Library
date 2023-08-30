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
#include <tuple>

#include <sustainml_cpp/config/Macros.hpp>
#include <sustainml_cpp/types/types.h>

#include <fastrtps/log/Log.h>


namespace sustainml {
namespace core {
namespace helper {

    template<typename T>
    void see_type(T)
    {
        EPROSIMA_LOG_INFO(CALLABLE, __PRETTY_FUNCTION__);
    }

    //! Expand the parameter pack in runtime
    //! It is mainly used before invoking user callback
    template <std::size_t... Ts>
    struct index {};

    template <std::size_t N, std::size_t... Ts>
    struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};

    template <std::size_t... Ts>
    struct gen_seq<0, Ts...> : index<Ts...> {};

} // namespace helper

    /**
    * @brief This class is used for registering an input callback
    * with a variable number of arguments and generic types.
    *
    * It is meant to be inherited by the actual SustainML nodes.
    */
    template <typename ..._TYPES>
    class Callable
    {
        using tuple = std::tuple<_TYPES*...>;

        template <size_t N>
        using get = typename std::tuple_element<N, tuple>::type;

        public:

        //! number of types in pack.
        static constexpr auto size = sizeof...(_TYPES);

        /**
        * @brief Register the user's callback
        */
        SUSTAINML_CPP_DLL_API constexpr void register_cb(std::function<void(_TYPES& ...)> fn)
        {
            //helper::see_type(fn);
            user_callback_ = fn;
        }

        /**
        * @brief Invokes the user callback with the arguments stored in user_cb_args_
        */
        template <std::size_t... Is>
        void invoke_user_cb(helper::index<Is...>)
        {
            user_callback_(*std::get<Is>(user_cb_args_)...);
        }

        /**
        * @brief Returns the arguments with which the user callback
        * will be later invoked
        *
        * @return Reference to the user callback args
        */
        tuple& get_user_cb_args()
        {
            return user_cb_args_;
        }

        private:

        std::function<void(_TYPES&...)> user_callback_;
        tuple user_cb_args_;
    };

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_CALLABLE_HPP
