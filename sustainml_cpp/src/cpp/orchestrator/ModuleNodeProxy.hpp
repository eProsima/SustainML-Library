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
 * @file ModuleNodeProxy.hpp
 */


#ifndef SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXY_HPP
#define SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXY_HPP

#include <memory>

#include "TaskDB.hpp"
#include "Helper.hpp"

#include <sustainml_cpp/orchestrator/OrchestratorNode.hpp>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>

using namespace eprosima::fastdds::dds;

namespace sustainml {
namespace orchestrator {

/**
 * @brief This class represents a proxy of the actual ModuleNode for the
 * Orchestrator.
 * It uses a contentfilter to get the states related only with this node.
 */
class ModuleNodeProxy
{
    /**
    * @brief Listener for the Module Node output
    */
    struct ModuleNodeProxyListener : public DataReaderListener
    {
        ModuleNodeProxyListener(
                    ModuleNodeProxy* parent);

        virtual ~ModuleNodeProxyListener();

        void on_data_available(
            eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
            eprosima::fastdds::dds::DataReader* reader,
            const eprosima::fastdds::dds::SubscriptionMatchedStatus & status) override;

        ModuleNodeProxy* parent;
    };

    /**
    * @brief Listener for the Module Node status
    */
    struct ModuleNodeProxyStatusListener : public DataReaderListener
    {
        ModuleNodeProxyStatusListener(
                    ModuleNodeProxy* parent);

        virtual ~ModuleNodeProxyStatusListener();

        void on_data_available(
            eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
            eprosima::fastdds::dds::DataReader* reader,
            const eprosima::fastdds::dds::SubscriptionMatchedStatus & status) override;

        ModuleNodeProxy* parent;
    };

public:

    using TaskDB_t = orchestrator::OrchestratorNode::TaskDB_t;

    ModuleNodeProxy(
        DomainParticipant* part,
        std::shared_ptr<TaskDB_t> task_db);

    /**
    * @brief Retrieves the Status of the node
    */
    const types::NodeStatus& get_status();

protected:

    types::NodeStatus status_;
    std::shared_ptr<TaskDB_t> task_db_;

    TypeSupport type_;
    DomainParticipant* participant_;
    Topic* topic_;
    ContentFilteredTopic* status_topic_;
    Subscriber* sub_;
    DataReader* datareader_;
    DataReader* status_datareader_;

    ModuleNodeProxyListener listener_;
    ModuleNodeProxyStatusListener status_listener_;
};

/**
* @brief TaskEncoder node proxy
*/
class TaskEncoderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapToNodeID_t<NodeID::ID_TASK_ENCODER> node_id_{};

public:
    TaskEncoderNodeProxy(
        DomainParticipant* part,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);
};

/**
* @brief Machine Learning node proxy
*/
class MLModelProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapToNodeID_t<NodeID::ID_MACHINE_LEARNING> node_id_{};

public:
    MLModelProviderNodeProxy(
        DomainParticipant* part,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);
};

/**
* @brief Hardware Resources node proxy
*/
class HardwareResourcesProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapToNodeID_t<NodeID::ID_HARDWARE_RESOURCES> node_id_{};

public:
    HardwareResourcesProviderNodeProxy(
        DomainParticipant* part,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);
};

/**
* @brief Carbon Footprint node proxy
*/
class CarbonFootprintProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapToNodeID_t<NodeID::ID_CARBON_FOOTPRINT> node_id_{};

public:
    CarbonFootprintProviderNodeProxy(
        DomainParticipant* part,
        std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);
};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXY_HPP
