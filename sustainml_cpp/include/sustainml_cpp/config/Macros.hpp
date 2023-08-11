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
 * @file Macros.hpp
 */

#ifndef SUSTAINMLCPP_CONFIG_MACROS_HPP
#define SUSTAINMLCPP_CONFIG_MACROS_HPP

// Dynamic linkage

#if defined(_WIN32)
    #define SUSTAINML_CPP_DLL_API __declspec( dllexport )
#else
    #define SUSTAINML_CPP_DLL_API
#endif// _WIN32

#endif //SUSTAINMLCPP_CONFIG_MACROS_HPP
