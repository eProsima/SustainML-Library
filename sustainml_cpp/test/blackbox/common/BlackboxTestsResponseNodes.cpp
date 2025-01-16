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

#include "BlackboxTests.hpp"

TEST(BlackboxTestsResponseNodes, MLModelMetadataNode)
{
    MLModelMetadataManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_ML_MODEL_METADATA));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_CARBON_FOOTPRINT));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}

TEST(BlackboxTestsResponseNodes, AppRequirementsNode)
{
    AppRequirementsManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_APP_REQUIREMENTS));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_CARBON_FOOTPRINT));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}

TEST(BlackboxTestsResponseNodes, CarbonFootprintNode)
{
    CarbonFootprintManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_CARBON_FOOTPRINT));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_APP_REQUIREMENTS));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}

TEST(BlackboxTestsResponseNodes, HWConstraintsNode)
{
    HWConstraintsManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_HW_CONSTRAINTS));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_APP_REQUIREMENTS));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}

TEST(BlackboxTestsResponseNodes, HWResourcesNode)
{
    HWResourcesManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_HW_RESOURCES));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_APP_REQUIREMENTS));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}

TEST(BlackboxTestsResponseNodes, MLModelNodeService)
{
    MLModelManagedNodeService node;

    TaskInjector<RequestTypeImplPubSubType> request("sustainml/request");
    TaskReceiver<ResponseTypeImplPubSubType> response("sustainml/response");

    node.start();

    request.wait_discovery(1);
    response.wait_discovery(1);

    RequestTypeImpl req;
    req.node_id(static_cast<long>(NodeID::ID_ML_MODEL));
    req.transaction_id(1);
    req.configuration("Test");
    request.inject_request(req);

    ResponseTypeImpl res;
    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_TRUE(res.configuration() == req.configuration());

    req.node_id(static_cast<long>(NodeID::ID_APP_REQUIREMENTS));
    req.transaction_id(1);
    req.configuration("Test2");
    request.inject_request(req);

    response.get_data(res, std::chrono::milliseconds(1000));

    ASSERT_FALSE(res.configuration() == req.configuration());
}
