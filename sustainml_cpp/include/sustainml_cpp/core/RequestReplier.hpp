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
 * @file RequestReplier.hpp
 */

#ifndef SUSTAINMLCPP_CORE_REQUESTREPLEIR_HPP
#define SUSTAINMLCPP_CORE_REQUESTREPLEIR_HPP

#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <sustainml_cpp/types/types.hpp>
#include <types/typesImplPubSubTypes.hpp>

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

namespace sustainml {
namespace core {

/**
 * @brief This class in charge of sending request or response
 * and listen for "response" or "request" respectively.
 *
 */
class RequestReplier
{

    friend class Node;
    friend class OrchestratorNode;

public:

    RequestReplier(
            std::function<void(void*)> callback,
            const char* topicw,
            const char* topicr,
            void* data);

    ~RequestReplier();

    /**
     * @brief Method used to send the response to the request in the Nodes.
     *
     * @param res Response message
     */
    void write_res(
            ResponseTypeImpl* res);

    /**
     * @brief Method used to send the request of configuration in the OrchestratorNode.
     *
     * @param req Request message
     */
    void write_req(
            RequestTypeImpl* req);

protected:

    std::function<void(void*)> callback_;
    const char* topicr_;
    const char* topicw_;
    void* data_;

    eprosima::fastdds::dds::DomainParticipant* participant_;

    eprosima::fastdds::dds::Publisher* publisher_;

    eprosima::fastdds::dds::Subscriber* subscriber_;

    eprosima::fastdds::dds::TypeSupport typeRes_;
    eprosima::fastdds::dds::TypeSupport typeReq_;

    eprosima::fastdds::dds::Topic* topicR_;
    eprosima::fastdds::dds::Topic* topicW_;

    eprosima::fastdds::dds::DataWriter* writer_;

    eprosima::fastdds::dds::DataReader* reader_;

private:

    class RequestReplyControlListener : public eprosima::fastdds::dds::DataReaderListener
    {
    public:

        RequestReplyControlListener(
                RequestReplier* node);

        virtual ~RequestReplyControlListener();

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

        RequestReplier* node_;
        size_t matched_;

    }
    listener_;
};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_REQUESTREPLEIR_HPP
