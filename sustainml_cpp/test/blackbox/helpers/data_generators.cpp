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

#include "../common/BlackboxTests.hpp"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <list>
#include <sstream>

std::list<AppRequirementsImpl> default_apprequirements_data_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<AppRequirementsImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                AppRequirementsImpl app_req;
                app_req.task_id(index);
                std::vector<std::string> requirements;
                requirements.push_back("req1");
                requirements.push_back("req2");
                requirements.push_back("req3");
                requirements.push_back("req4");
                requirements.push_back("req5");
                app_req.app_requirements(requirements);
                ++index;
                return app_req;
            });

    return returnedValue;
}

std::list<CO2FootprintImpl> default_co2footprint_data_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<CO2FootprintImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                CO2FootprintImpl co2f;
                co2f.task_id(index);
                std::stringstream ss;
                ss << "co2fWorld " << index;
                co2f.carbon_footprint(index * 1.0);
                co2f.carbon_intensity(22.0);
                co2f.energy_consumption(index * 30.0);
                ++index;
                return co2f;
            });

    return returnedValue;
}

std::list<HWConstraintsImpl> default_hwconstraints_task_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<HWConstraintsImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                HWConstraintsImpl hw_cons;
                hw_cons.task_id(index);
                hw_cons.max_memory_footprint(index);
                ++index;
                return hw_cons;
            });

    return returnedValue;
}

std::list<HWResourceImpl> default_hwresource_task_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<HWResourceImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                HWResourceImpl hw_res;
                hw_res.task_id(index);
                std::stringstream ss;
                ss << "HW Description of Task id: " << index;
                hw_res.hw_description(ss.str());
                ++index;
                return hw_res;
            });

    return returnedValue;
}

std::list<MLModelMetadataImpl> default_modelmetadata_task_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<MLModelMetadataImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                MLModelMetadataImpl enc_task;
                enc_task.task_id(index);
                std::vector<std::string> keywords;
                keywords.push_back("EXAMPLE");
                keywords.push_back("KEYWORD");
                enc_task.keywords(keywords);
                ++index;
                return enc_task;
            });

    return returnedValue;
}

std::list<MLModelImpl> default_mlmodel_task_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<MLModelImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                MLModelImpl ml_model;
                ml_model.task_id(index);
                std::stringstream ss;
                ss << "Machine learning model for Task ID " << index;
                ml_model.model(ss.str());
                ++index;
                return ml_model;
            });

    return returnedValue;
}

std::list<UserInputImpl> default_userinput_task_generator(
        size_t max)
{
    uint16_t index = 0;
    size_t maximum = max ? max : 20;
    std::list<UserInputImpl> returnedValue(maximum);

    std::generate(returnedValue.begin(), returnedValue.end(), [&index]
            {
                UserInputImpl ui;
                ui.task_id(index);
                std::stringstream problem_definition;
                problem_definition << "Problem Description of Task id: " << index;
                ui.problem_definition(problem_definition.str());
                std::stringstream task_name;
                task_name << "initial task";
                ui.task_name(task_name.str());
                ++index;
                return ui;
            });

    return returnedValue;
}
