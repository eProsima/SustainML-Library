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

%module(directors="1", threads="1") sustainml_swig

// Handle exceptions on python callbacks and send them back to C++ so that they can be catched
// Also, add some meaningful description of the error
%feature("director:except") {
  if ($error != NULL) {
    PyObject *exc, *val, *tb;
    PyErr_Fetch(&exc, &val, &tb);
    PyErr_NormalizeException(&exc, &val, &tb);
    std::string err_msg("In method '$symname': ");

    PyObject* exc_str = PyObject_GetAttrString(exc, "__name__");
    err_msg += PyUnicode_AsUTF8(exc_str);
    Py_XDECREF(exc_str);

    if (val != NULL)
    {
      PyObject* val_str = PyObject_Str(val);
      err_msg += ": ";
      err_msg += PyUnicode_AsUTF8(val_str);
      Py_XDECREF(val_str);
    }

    Py_XDECREF(exc);
    Py_XDECREF(val);
    Py_XDECREF(tb);

    Swig::DirectorMethodException::raise(err_msg.c_str());
  }
}

// If using windows in debug, it would try to use python_d, which would not be found.
%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif
#include <exception>
%}

// Macro delcarations
// Any macro used on the header files will give an error if it is not redefined here
#define SUSTAINML_CPP_DllAPI

// SWIG helper modules
%include cpointer.i
%include stdint.i
%include std_array.i
%include std_list.i
%include std_string.i
%include std_shared_ptr.i
%include std_vector.i
%include typemaps.i

// This ensures that the returned string references can be used with the string API
// Otherwise, they will be wrapped objects without API
%typemap(out) std::string& {
  $result = SWIG_From_std_string(*$1);
}

%template(std_string_vec) std::vector<std::string>;

// Definition of internal types
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

// IMPORTANT: the order of these includes is relevant, and must keep same order of cpp declarations.
%include "sustainml_swig/types/types.i"

%include "sustainml_swig/core/Callable.i"
%include "sustainml_swig/core/Constants.i"
%include "sustainml_swig/core/Node.i"

%include "sustainml_swig/nodes/AppRequirementsNode.i"
%include "sustainml_swig/nodes/CarbonFootprintNode.i"
%include "sustainml_swig/nodes/HardwareConstraintsNode.i"
%include "sustainml_swig/nodes/HardwareResourcesNode.i"
%include "sustainml_swig/nodes/MLModelNode.i"
%include "sustainml_swig/nodes/MLModelMetadataNode.i"
%include "sustainml_swig/nodes/OrchestratorNode.i"
