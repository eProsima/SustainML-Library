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

#include "../common/BlackboxTests.hpp"

// Managed node helpers
#include "../api/ManagedNode.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/domain/qos/RequesterQos.hpp>
#include <fastdds/dds/rpc/exceptions.hpp>

#include <common/Common.hpp>

// RPC generated headers
#include <types/SustainMLServiceClient.hpp>
#include <types/SustainMLService.hpp>

using namespace eprosima::fastdds::dds;

namespace {
DomainParticipant* make_client_participant(
        const char* name)
{
    auto* dpf = DomainParticipantFactory::get_instance();

    DomainParticipantQos pqos = PARTICIPANT_QOS_DEFAULT;
    pqos.name(name);

    const uint32_t domain = sustainml::common::parse_sustainml_env(0);

    auto* p = dpf->create_participant(domain, pqos);
    return p;
}

void destroy_participant(
        DomainParticipant* p)
{
    if (!p)
    {
        return;
    }

    auto* dpf = DomainParticipantFactory::get_instance();
    p->delete_contained_entities();
    dpf->delete_participant(p);
}

template <typename FutureT>
void wait_ready_or_fail(
        FutureT& fut,
        const char* tag,
        std::chrono::seconds max = std::chrono::seconds(3))
{
    const auto deadline = std::chrono::steady_clock::now() + max;

    while (true)
    {
        auto st = fut.wait_for(std::chrono::milliseconds(200));
        if (st == std::future_status::ready)
        {
            return;
        }
        if (std::chrono::steady_clock::now() > deadline)
        {
            FAIL() << tag << " did not complete in time";
        }
    }
}

} // namespace

TEST(BlackboxTestsRPCResponseNodes, MLModelMetadataNode)
{
    // Start the MLModelMetadata node (RPC server side)
    MLModelMetadataManagedNode node;
    node.start();

    // Client side participant (separate from node participant)
    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_METADATA");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    // POSITIVE: correct service responds
    auto client =
            create_MLModelMetadataServiceClient(*client_participant, "MLModelMetadataService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "MLModelMetadataService.update_configuration");

    // If it’s ready, it must return the same config (same intent as old test)
    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    // NEGATIVE: wrong target should not “succeed”
    auto wrong_client =
            create_CarbonFootprintServiceClient(*client_participant, "CarbonFootprintService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    // Here we accept either:
    // - timeout (no server answers), OR
    // - ready but throws (RPC layer decides quickly there is no server)
    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        // If it times out, that also means “wrong target did not succeed quickly”
        EXPECT_EQ(st, std::future_status::timeout);
    }

    // Cleanup in order
    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}

TEST(BlackboxTestsRPCResponseNodes, AppRequirementsNode)
{
    AppRequirementsManagedNode node;
    node.start();

    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_APP_REQ");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    auto client =
            create_AppRequirementsServiceClient(*client_participant, "AppRequirementsService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "AppRequirementsService.update_configuration");

    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    auto wrong_client =
            create_CarbonFootprintServiceClient(*client_participant, "CarbonFootprintService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        EXPECT_EQ(st, std::future_status::timeout);
    }

    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}

TEST(BlackboxTestsRPCResponseNodes, CarbonFootprintNode)
{
    CarbonFootprintManagedNode node;
    node.start();

    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_CO2");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    auto client =
            create_CarbonFootprintServiceClient(*client_participant, "CarbonFootprintService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "CarbonFootprintService.update_configuration");

    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    auto wrong_client =
            create_AppRequirementsServiceClient(*client_participant, "AppRequirementsService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        EXPECT_EQ(st, std::future_status::timeout);
    }

    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}

TEST(BlackboxTestsRPCResponseNodes, HWConstraintsNode)
{
    HWConstraintsManagedNode node;
    node.start();

    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_HWC");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    auto client =
            create_HWConstraintsServiceClient(*client_participant, "HWConstraintsService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "HWConstraintsService.update_configuration");

    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    auto wrong_client =
            create_MLModelServiceClient(*client_participant, "MLModelService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        EXPECT_EQ(st, std::future_status::timeout);
    }

    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}

TEST(BlackboxTestsRPCResponseNodes, HWResourcesNode)
{
    HWResourcesManagedNode node;
    node.start();

    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_HWR");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    auto client =
            create_HWResourcesServiceClient(*client_participant, "HWResourcesService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "HWResourcesService.update_configuration");

    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    auto wrong_client =
            create_MLModelMetadataServiceClient(*client_participant, "MLModelMetadataService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        EXPECT_EQ(st, std::future_status::timeout);
    }

    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}

TEST(BlackboxTestsRPCResponseNodes, MLModelNodeService)
{
    MLModelManagedNode node;
    node.start();

    DomainParticipant* client_participant = make_client_participant("RPC_CLIENT_ML");
    ASSERT_NE(client_participant, nullptr);

    RequesterQos rqos;

    auto client =
            create_MLModelServiceClient(*client_participant, "MLModelService", rqos);
    ASSERT_NE(client, nullptr);

    auto fut = client->update_configuration("TestConfig");

    wait_ready_or_fail(fut, "MLModelService.update_configuration");

    try
    {
        const auto value = fut.get();
        EXPECT_EQ(value, "TestConfig");
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception: " << e.what();
    }

    auto wrong_client =
            create_HWConstraintsServiceClient(*client_participant, "HWConstraintsService", rqos);
    ASSERT_NE(wrong_client, nullptr);

    auto bad_fut = wrong_client->update_configuration("WrongTarget");

    auto st = bad_fut.wait_for(std::chrono::milliseconds(3000));

    if (st == std::future_status::ready)
    {
        bool threw = false;
        try
        {
            (void)bad_fut.get();
        }
        catch (...)
        {
            threw = true;
        }
        EXPECT_TRUE(threw) << "Expected the wrong-service RPC call to fail (throw), but it returned normally.";
    }
    else
    {
        EXPECT_EQ(st, std::future_status::timeout);
    }

    wrong_client.reset();
    client.reset();
    destroy_participant(client_participant);
    node.stop();
}
