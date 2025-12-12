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
 * @file GenericServiceNode.cpp
 */

 #ifndef SUSTAINML_CORE_GENERICSERVICENODEIMPL_HPP
#define SUSTAINML_CORE_GENERICSERVICENODEIMPL_HPP

#include <string>
#include <iostream>

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

template <typename ServerBase>
class GenericServiceNodeImpl : public ServerBase
{
public:
    GenericServiceNodeImpl(NodeImpl& node, const char* tag)
        : node_(node)
        , tag_(tag)
    {
    }

    std::string update_configuration(
            const eprosima::fastdds::dds::rpc::RpcRequest& /*info*/,
            const std::string& configuration) override
    {
        std::cout << "[RPC SERVER/" << tag_ << "] update_configuration cfg='"
                  << configuration << "'\n";

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
        catch (...)
        {
            throw ::InternalError("update_configuration: unknown error");
        }

        // If your listener reports failures via res.success():
        if (!res.success())
        {
            // If you have an error string field, put it here. If not:
            throw ::InternalError("update_configuration: res.success==false");
        }

        return res.configuration();
    }

    std::string send_data(
            const eprosima::fastdds::dds::rpc::RpcRequest& /*info*/,
            const std::string& data) override
    {
        (void)data;

        // If not implemented yet, raise InternalError (matches IDL)
        throw ::InternalError("send_data not implemented");
    }

private:
    NodeImpl& node_;
    const char* tag_;
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
