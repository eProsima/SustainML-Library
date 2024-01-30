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
 * @file ModuleNodeProxyFactory.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXYFACTORY_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXYFACTORY_HPP

#include <memory>

#include "Helper.hpp"
#include "ModuleNodeProxy.hpp"

#include <sustainml_cpp/types/types.h>


namespace sustainml {
namespace orchestrator {

/**
* @brief This class acts as a factory for creating new ModuleProxies.
*/
struct ModuleNodeProxyFactory
{
    static void make_node_proxy(
            const NodeID &id,
            OrchestratorNode* orchestrator,
            std::shared_ptr<ModuleNodeProxy::TaskDB_t> db,
            ModuleNodeProxy* &node_proxy)
    {
        switch (id)
        {
            case NodeID::ID_TASK_ENCODER:
            {
                node_proxy = new TaskEncoderNodeProxy(orchestrator, db);
                break;
            }
            case NodeID::ID_MACHINE_LEARNING:
            {
                node_proxy = new MLModelProviderNodeProxy(orchestrator, db);
                break;
            }
            case NodeID::ID_HARDWARE_RESOURCES:
            {
                node_proxy = new HardwareResourcesProviderNodeProxy(orchestrator, db);
                break;
            }
            case NodeID::ID_CARBON_FOOTPRINT:
            {
                node_proxy = new CarbonFootprintProviderNodeProxy(orchestrator, db);
                break;
            }
            default:
            {
                EPROSIMA_LOG_ERROR_IMPL_(MODULE_NODE_PROXY_FACTORY, "Invalid node ID");
                break;
            }
        }
    }
};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXYFACTORY_HPP
