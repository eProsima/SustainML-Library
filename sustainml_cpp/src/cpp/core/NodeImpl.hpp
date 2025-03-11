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
 * @file NodeImpl.hpp
 */

#ifndef SUSTAINMLCPP_CORE_NODE_IMPL_HPP
#define SUSTAINMLCPP_CORE_NODE_IMPL_HPP

#include <sustainml_cpp/config/Macros.hpp>
#include <sustainml_cpp/types/types.hpp>

#include <core/Options.hpp>
#include <core/RequestReplyListener.hpp>
#include <core/RequestReplier.hpp>
#include <types/typesImplPubSubTypes.hpp>

#include <utility>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>

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

#define STATUS_WRITER_IDX 0
#define OUTPUT_WRITER_IDX 1

namespace sustainml {
namespace core {

class Dispatcher;
class Node;
struct Options;

/**
 * @brief This abstract class is the principal class of the project.
 * Handles the DDS comunications, aggregates the dispatcher and provides
 * the main methods for interacting with the user.
 *
 * This class is meant to be inherited by the different
 * SustainML Nodes.
 */
class NodeImpl
{
    friend class Node;
    friend class Dispatcher;

    template<class T> friend class SamplesQueue;
    template<class T> friend class NodeListener;

public:

    NodeImpl(
            Node* node,
            const std::string& name);

    NodeImpl(
            Node* node,
            const std::string& name,
            const Options& opts);

    NodeImpl(
            Node* node,
            const std::string& name,
            RequestReplyListener& req_res_listener);

    NodeImpl(
            Node* node,
            const std::string& name,
            const Options& opts,
            RequestReplyListener& req_res_listener);

    ~NodeImpl();

    /**
     * @brief Called by the user to run the run.
     */
    void spin();

    /**
     * @brief Stops the execution of the node.
     */
    static void terminate();

protected:

    /**
     * @brief Starts a new subscription (DataReader) in the
     * given topic.
     *
     * @param topic The topic name
     * @param type_name The type name
     * @param listener Listener object inheriting from DataReaderListener
     * @param opts Options object with the Subscription configuration
     */
    bool initialize_subscription(
            const char* topic_name,
            const char* type_name,
            eprosima::fastdds::dds::DataReaderListener* listener,
            const Options& opts);

    /**
     * @brief Starts a new publication (DataWriter) in the
     * given topic.
     *
     * @param topic The topic name
     * @param type_name The type name
     * @param opts Options object with the Publication configuration
     */
    bool initialize_publication(
            const char* topic_name,
            const char* type_name,
            const Options& opts);

    /**
     * @brief Publishes the internal status of the node to DDS.
     */
    void publish_node_status();

    Node* node_;

    std::shared_ptr<Dispatcher> dispatcher_;

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Publisher* publisher_;

    eprosima::fastdds::dds::Subscriber* subscriber_;

    std::vector<eprosima::fastdds::dds::Topic*> topics_;

    // Status writer is always the first
    std::vector<eprosima::fastdds::dds::DataWriter*> writers_;

    std::vector<eprosima::fastdds::dds::DataReader*> readers_;

    std::mutex spin_mtx_;

    static std::condition_variable spin_cv_;

    static std::atomic<bool> terminate_;

    NodeStatusImpl node_status_;

    RequestReplier* req_res_;

    RequestReplyListener& req_res_listener_;

    RequestTypeImpl req_data_;

private:

    /**
     * @brief Getter for the dispatcher
     *
     * @return A weak pointer to the Dispatcher object
     */
    std::weak_ptr<Dispatcher> get_dispatcher()
    {
        return dispatcher_;
    }

    /**
     * @brief Initialize the class optionally from a set of Options
     *
     * @param opts Desired options
     */
    bool init(
            const std::string& name,
            const Options& opts = Options());

    class NodeControlListener : public eprosima::fastdds::dds::DataReaderListener
    {
    public:

        NodeControlListener(
                NodeImpl* node);

        virtual ~NodeControlListener();

        /**
         * @brief Callback executed when a new sample is available on the DataReader.
         *
         * @param reader The DataReader having new available samples.
         */
        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader);

        /**
         * @brief Callback executed when a DataReader matching status change.
         *
         * @param reader The DataReader.
         * @param status The status of the subscription.
         */
        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& status);

    private:

        NodeImpl* node_;
        size_t matched_;

    }
    control_listener_;
};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_NODE_IMPL_HPP
