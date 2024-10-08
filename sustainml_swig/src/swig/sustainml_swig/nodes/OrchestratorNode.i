// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
// Binding for class OrchestratorNode
////////////////////////////////////////////////////////

// Any macro used on the header files will give an error if it is not redefined here
#define SUSTAINML_CPP_DLL_API
#define SWIG_WRAPPER
#define GEN_API_VER 2

%include <std_pair.i>

%feature("director") sustainml::orchestrator::OrchestratorNodeHandle;

%template(sustainml_pair) std::pair<types::TaskId, types::UserInput*>;

%{
#include <sustainml_cpp/types/types.h>
#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>
%}

%feature("director") types::AppRequirements;
%feature("director") types::CO2Footprint;
%feature("director") types::HWConstraints;
%feature("director") types::HWResource;
%feature("director") types::MLModelMetadata;
%feature("director") types::MLModel;

// Retrieve the pointer to the C++ object
%inline %{
    types::AppRequirements* cast_to_AppRequirements(void* ptr) {
        return static_cast<types::AppRequirements*>(ptr);
    }

    types::CO2Footprint* cast_to_CO2Footprint(void* ptr) {
        return static_cast<types::CO2Footprint*>(ptr);
    }

    types::HWConstraints* cast_to_HWConstraints(void* ptr) {
        return static_cast<types::HWConstraints*>(ptr);
    }

    types::HWResource* cast_to_HWResource(void* ptr) {
        return static_cast<types::HWResource*>(ptr);
    }

    types::MLModel* cast_to_MLModel(void* ptr) {
        return static_cast<types::MLModel*>(ptr);
    }

    types::MLModelMetadata* cast_to_MLModelMetadata(void* ptr) {
        return static_cast<types::MLModelMetadata*>(ptr);
    }

    void* new_voidp() {
        return new void*;
    }
%}

// Include the class interfaces
%include <sustainml_cpp/types/types.h>
%include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>
