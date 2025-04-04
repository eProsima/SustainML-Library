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

////////////////////////////////////////////////////////
// Binding for class types
////////////////////////////////////////////////////////

// Any macro used on the header files will give an error if it is not redefined here
#define eProsima_user_DllExport
#define GEN_API_VER 2

%include std_string.i

// Ignore overloaded methods that have no application on Python
// Warnings regarding equality operators and stuff
%ignore *::operator=;
%ignore operator<<;

%{
#include <sustainml_cpp/types/types.hpp>
%}

%extend std::vector<uint8_t>
{
    const uint8_t* get_buffer() const
    {
        return self->data();
    }
}

%template(uint8_t_vector) std::vector<uint8_t>;

// Include the class interfaces
%include <sustainml_cpp/types/types.hpp>
