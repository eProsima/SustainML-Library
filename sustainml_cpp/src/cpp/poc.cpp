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
 * @file poc.cpp
 */

#include <csignal>
#include <cstdlib>
#include <functional>

#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>

void cb(types::MLModel& model, types::UserInput& ui, types::HWResource& hw, types::NodeStatus& status, types::CO2Footprint& output)
{
    std::cout << "User Received task IDs: " << model.task_id()  << " " << ui.task_id() << " " << hw.task_id()
    << " DATA -> " << model.task_id() << " " << ui.task_id() << " " << hw.task_id() << std::endl;
}

int main()
{
    signal(SIGINT, [](int signum)
            {
                std::cout << "SIGINT received, stopping execution." << std::endl;
                static_cast<void>(signum); sustainml::core::Node::terminate();
            });

    sustainml::co2_tracker_module::CarbonFootprintNode n;

    n.register_cb(&cb);

    n.spin();

    return EXIT_SUCCESS;
}
