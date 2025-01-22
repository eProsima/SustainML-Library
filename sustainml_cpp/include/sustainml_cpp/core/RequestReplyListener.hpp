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
 * @file RequestReplyListener.hpp
 */

#ifndef SUSTAINMLCPP_CORE_REQUESTREPLYLISTENER_HPP
#define SUSTAINMLCPP_CORE_REQUESTREPLYLISTENER_HPP

#include <sustainml_cpp/types/types.hpp>

namespace sustainml {
namespace core {

/**
 * @brief This class implement a virtual function for the configuration of each node.
 * Each node will need to inherit it and implement their own.
 * End user to implement specific callbacks to certain actions.
 *
 */
class RequestReplyListener
{

public:

    /**
     * @brief Constructor
     */
    RequestReplyListener()
    {
    }

    /**
     * @brief Destructor
     */
    virtual ~RequestReplyListener()
    {
    }

    /**
     * Virtual method to be called when a configuration request is received.
     *
     * @param req The request message
     * @param res The response message to send
     */
    virtual void on_configuration_request(
            types::RequestType& req,
            types::ResponseType& res)
    {
    }

};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_REQUESTREPLYLISTENER_HPP
