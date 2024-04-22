// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
            const char* name,
            bool need_to_publish_baseline);

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

    /**
     * @brief Returns whether a proxy is publishing baseline data or not
     */
    bool publishes_baseline()
    {
        return publish_baseline_;
    }

    /**
     * @brief Starts the publication of new iteration data
     */
    virtual void publish_data_for_iteration(
            const types::TaskId& task_id) = 0;

protected:

    /**
     * @brief Publihes iteration data
     */
    template<typename T>
    void publish_data_for_iteration_(
            const types::TaskId& task_id, T* data);

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
    void notify_new_node_ouput();

    /**
     * @brief Prepare a new entry and resets task manager counter
     * to a certain task_id. This is useful in case the Orchestrator
     * is a late joiner.
     * @warning This method is not thread safe
     */
    void reset_and_prepare_task_id_nts(
            const types::TaskId& task_id);

    /**
     * @brief Stores an untyped data into the DB
     */
    virtual void store_data_in_db() = 0;

    /**
     * @brief Get a pointer to a stack allocated
     * temporary impl untyped data
     */
    virtual void* get_tmp_impl_untyped_data() = 0;

    /**
     * @brief Get a pointer to a stack allocated
     * temporary untyped data
     */
    virtual void* get_tmp_untyped_data() = 0;

    const char* name_;
    const NodeID node_id_;
    bool publish_baseline_;

    OrchestratorNode* orchestrator_;

    types::NodeStatus status_;
    std::shared_ptr<TaskDB_t> task_db_;

    TypeSupport type_;
    Topic* node_output_topic_;
    Topic* baseline_topic_;
    ContentFilteredTopic* filtered_status_topic_;
    DataReader* node_output_datareader_;
    DataReader* status_datareader_;
    DataWriter* baseline_writer_;

    ModuleNodeProxyListener listener_;
    ModuleNodeProxyStatusListener status_listener_;
};

/**
 * @brief App requirements node proxy
 */
class AppRequirementsNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_APP_REQUIREMENTS> node_id_to_type_id_{};

public:

    AppRequirementsNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~AppRequirementsNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief Carbon Footprint node proxy
 */
class CarbonFootprintNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_CARBON_FOOTPRINT> node_id_to_type_id_{};

public:

    CarbonFootprintNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~CarbonFootprintNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief Hardware Resources node proxy
 */
class HardwareConstraintsNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_HW_CONSTRAINTS> node_id_to_type_id_{};

public:

    HardwareConstraintsNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~HardwareConstraintsNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief Hardware Resources node proxy
 */
class HardwareResourcesNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_HW_RESOURCES> node_id_to_type_id_{};

public:

    HardwareResourcesNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~HardwareResourcesNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief MLModelMetadata node proxy
 */
class MLModelMetadataNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_ML_MODEL_METADATA> node_id_to_type_id_{};

public:

    MLModelMetadataNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~MLModelMetadataNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;
};

/**
 * @brief ML Model node proxy
 */
class MLModelProviderNodeProxy : public ModuleNodeProxy
{
    static constexpr MapFromNodeIDToType_t<NodeID::ID_ML_MODEL> node_id_to_type_id_{};

public:

    MLModelProviderNodeProxy(
            OrchestratorNode* orchestrator,
            std::shared_ptr<orchestrator::OrchestratorNode::TaskDB_t> task_db,
            bool need_to_publish_baseline);

    virtual ~MLModelProviderNodeProxy()
    {
    }

    void publish_data_for_iteration(
            const types::TaskId& task_id) override;

protected:

    void store_data_in_db() override;

    inline void* get_tmp_impl_untyped_data() override
    {
        return tmp_data_.get_impl();
    }

    inline void* get_tmp_untyped_data() override
    {
        return &tmp_data_;
    }

private:

    decltype(node_id_to_type_id_)::type tmp_data_;

};

} // namespace orchestrator
} // namespace sustainml

#endif // SUSTAINMLCPP_NODES_ORCHESTRATOR_MODULENODEPROXY_HPP
