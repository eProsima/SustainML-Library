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
#include <sustainml_cpp/core/Constants.hpp>
#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>
#include <sustainml_cpp/types/types.h>
%}

%feature("director") types::AppRequirements;
%feature("director") types::CO2Footprint;
%feature("director") types::HWConstraints;
%feature("director") types::HWResource;
%feature("director") types::MLModelMetadata;
%feature("director") types::MLModel;

// Retrieve the pointer to the C++ object
%inline %{
    types::AppRequirements* get_app_requirements(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::AppRequirements* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_APP_REQUIREMENTS, data))
        {
            node = static_cast<types::AppRequirements*>(data);
        }
        return node;
    }

    types::CO2Footprint* get_carbontracker(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::CO2Footprint* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_CARBON_FOOTPRINT, data))
        {
            node = static_cast<types::CO2Footprint*>(data);
        }
        return node;
    }

    types::HWConstraints* get_hw_constraints(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::HWConstraints* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_HW_CONSTRAINTS, data))
        {
            node = static_cast<types::HWConstraints*>(data);
        }
        return node;
    }

    types::HWResource* get_hw_provider(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::HWResource* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_HW_RESOURCES, data))
        {
            node = static_cast<types::HWResource*>(data);
        }
        return node;
    }

    types::MLModelMetadata* get_model_metadata(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::MLModelMetadata* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_ML_MODEL_METADATA, data))
        {
            node = static_cast<types::MLModelMetadata*>(data);
        }
        return node;
    }

    types::MLModel* get_model_provider(
        sustainml::orchestrator::OrchestratorNode* orchestrator,
        const types::TaskId& task_id)
    {
        void* data = nullptr;
        types::MLModel* node = nullptr;
        if (sustainml::RetCode_t::RETCODE_OK == orchestrator->get_task_data(
                task_id, sustainml::NodeID::ID_ML_MODEL, data))
        {
            node = static_cast<types::MLModel*>(data);
        }
        return node;
    }

    types::TaskId* get_task_id(
        const sustainml::NodeID node_id,
        void* data)
    {
        types::TaskId* task_id = nullptr;
        types::AppRequirements* requirements = nullptr;
        types::CO2Footprint* carbon = nullptr;
        types::HWConstraints* hw_constraints = nullptr;
        types::HWResource* hw_resources = nullptr;
        types::MLModel* model = nullptr;
        types::MLModelMetadata* metadata = nullptr;
        types::UserInput* input = nullptr;

        switch (node_id)
        {
            case sustainml::NodeID::ID_APP_REQUIREMENTS:
                requirements = static_cast<types::AppRequirements*>(data);
                task_id = &requirements->task_id();
                break;
            case sustainml::NodeID::ID_CARBON_FOOTPRINT:
                carbon = static_cast<types::CO2Footprint*>(data);
                task_id = &carbon->task_id();
                break;
            case sustainml::NodeID::ID_HW_CONSTRAINTS:
                hw_constraints = static_cast<types::HWConstraints*>(data);
                task_id = &hw_constraints->task_id();
                break;
            case sustainml::NodeID::ID_HW_RESOURCES:
                hw_resources = static_cast<types::HWResource*>(data);
                task_id = &hw_resources->task_id();
                break;
            case sustainml::NodeID::ID_ML_MODEL:
                model = static_cast<types::MLModel*>(data);
                task_id = &model->task_id();
                break;
            case sustainml::NodeID::ID_ML_MODEL_METADATA:
                metadata = static_cast<types::MLModelMetadata*>(data);
                task_id = &metadata->task_id();
                break;
            default:
                break;
        }
        return task_id;
    }
%}

// Include the class interfaces
%include <sustainml_cpp/types/types.h>
%include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>
