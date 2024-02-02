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

        virtual ~ModuleNodeProxyListener()
        {
        }

        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& status) override;

        ModuleNodeProxy* proxy_parent_;
    };

    /**
     * @brief Listener for the Module Node status
     */
    struct ModuleNodeProxyStatusListener : public DataReaderListener
    {
        ModuleNodeProxyStatusListener(
                ModuleNodeProxy* parent);

        virtual ~ModuleNodeProxyStatusListener()
        {
        }

        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& status) override;

        ModuleNodeProxy* proxy_parent_;
    };

public:

    using TaskDB_t = orchestrator::OrchestratorNode::TaskDB_t;

    ModuleNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<TaskDB_t> task_db,
            const char* name);

    virtual ~ModuleNodeProxy();

    /**
     * @brief Retrieves the Status of the node
     */
    const types::NodeStatus& get_status();

    /**
     * @brief Retrieves the Status of the node
     */
    void set_status(
            const types::NodeStatus&);

protected:

    /**
     * @brief Notifies the Orchestrator about
     * a new change in the status of this Proxy
     */
    void notify_status_change();

    /**
     * @brief Notifies the Orchestrator about
     * a new output available in this node to store
     * it into the db
     */
    void notify_new_node_ouput(
            void* data);

    /**
     * @brief Stores an untyped data into the DB
     */
    virtual void store_data_in_db() = 0;

    /**
     * @brief Get a pointer to a stack allocated
     * temporary typed data
     */
    virtual void* get_tmp_impl_typed_data() = 0;

    const char* name_;
    const NodeID node_id_;

    OrchestratorNode* orchestrator_;

    types::NodeStatus status_;
    std::shared_ptr<TaskDB_t> task_db_;

    TypeSupport type_;
    Topic* node_output_topic_;
    ContentFilteredTopic* filtered_status_topic_;
    DataReader* node_output_datareader_;
    DataReader* status_datareader_;

    ModuleNodeProxyListener listener_;
    ModuleNodeProxyStatusListener status_listener_;
};

/**
 * @brief TaskEncoder node proxy
 */
class TaskEncoderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_TASK_ENCODER> node_id_to_type_id_{};

public:

    TaskEncoderNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);

    virtual ~TaskEncoderNodeProxy()
    {
    }

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_typed_data() override
    {
        return tmp_data_.get_impl();
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief Machine Learning node proxy
 */
class MLModelProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_MACHINE_LEARNING> node_id_to_type_id_{};

public:

    MLModelProviderNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);

    virtual ~MLModelProviderNodeProxy()
    {
    }

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_typed_data() override
    {
        return tmp_data_.get_impl();
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;

};

/**
 * @brief Hardware Resources node proxy
 */
class HardwareResourcesProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_HARDWARE_RESOURCES> node_id_to_type_id_{};

public:

    HardwareResourcesProviderNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);

    virtual ~HardwareResourcesProviderNodeProxy()
    {
    }

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_typed_data() override
    {
        return tmp_data_.get_impl();
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief Carbon Footprint node proxy
 */
class CarbonFootprintProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_CARBON_FOOTPRINT> node_id_to_type_id_{};

public:

    CarbonFootprintProviderNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db);

    virtual ~CarbonFootprintProviderNodeProxy()
    {
    }

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_typed_data() override
    {
        return tmp_data_.get_impl();
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXY_HPP
