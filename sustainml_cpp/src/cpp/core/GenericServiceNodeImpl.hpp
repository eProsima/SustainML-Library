// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file GenericServiceNodeImpl.hpp
 */

#ifndef SUSTAINML_CORE_GENERICSERVICENODEIMPL_HPP
#define SUSTAINML_CORE_GENERICSERVICENODEIMPL_HPP

#include <string>

#include <core/NodeImpl.hpp>
#include <types/types.hpp>
#include <types/SustainMLServiceServer.hpp>

namespace eprosima {
namespace fastdds {
namespace dds {
namespace rpc {
class RpcRequest;
} // namespace rpc
} // namespace dds
} // namespace fastdds
} // namespace eprosima

namespace sustainml {
namespace core {

template<typename ServerBase>
class GenericServiceNodeImpl : public ServerBase
{
public:

    GenericServiceNodeImpl(
            NodeImpl& node)
        : node_(node)
    {
    }

    std::string update_configuration(
            const eprosima::fastdds::dds::rpc::RpcRequest& /*info*/,
            const std::string& configuration) override
    {
        if (node_.shutting_down())
        {
            throw ::InternalError("update_configuration: node is shutting down");
        }

        types::RequestType req;
        types::ResponseType res;

        req.configuration(configuration);

        try
        {
            node_.request_listener().on_configuration_request(req, res);
        }
        catch (const std::exception& e)
        {
            throw ::InternalError(std::string("update_configuration: ") + e.what());
        }

        return res.configuration();
    }

private:

    NodeImpl& node_;
};

using AppRequirementsServiceNodeImpl =
        GenericServiceNodeImpl<AppRequirementsServiceServer_IServerImplementation>;
using HWConstraintsServiceNodeImpl =
        GenericServiceNodeImpl<HWConstraintsServiceServer_IServerImplementation>;
using HWResourcesServiceNodeImpl =
        GenericServiceNodeImpl<HWResourcesServiceServer_IServerImplementation>;
using CarbonFootprintServiceNodeImpl =
        GenericServiceNodeImpl<CarbonFootprintServiceServer_IServerImplementation>;
using MLModelMetadataServiceNodeImpl =
        GenericServiceNodeImpl<MLModelMetadataServiceServer_IServerImplementation>;
using MLModelServiceNodeImpl =
        GenericServiceNodeImpl<MLModelServiceServer_IServerImplementation>;

} // namespace core
} // namespace sustainml

#endif // SUSTAINML_CORE_GENERICSERVICENODEIMPL_HPP
