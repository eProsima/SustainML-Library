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
 * @file Node.hpp
 */


#ifndef SUSTAINMLCPP_CORE_NODE_HPP
#define SUSTAINMLCPP_CORE_NODE_HPP

#include <sustainml_cpp/config/Macros.hpp>
#include <sustainml_cpp/types/types.h>

#include <utility>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define STATUS_WRITER_IDX 0
#define OUTPUT_WRITER_IDX 1

namespace eprosima {
namespace fastdds {
namespace dds {

class DataWriter;
class DataReaderListener;

} // namespace dds
} // namespace fastdds
} // namespace eprosima

namespace sustainml {
namespace core {

class NodeImpl;
class NodeControlListener;
class Dispatcher;
struct Options;

/**
 * @brief This abstract class is the principal class of the project.
 * Handles the DDS comunications, aggregates the dispatcher and provides
 * the main methods for interacting with the user.
 *
 * This class is meant to be inherited by the different
 * SustainML Nodes.
 */
class Node
{

    friend class Dispatcher;
    template<class T> friend class SamplesQueue;
    template<class T> friend class NodeListener;

public:

    SUSTAINML_CPP_DLL_API Node(
            const std::string& name);

    SUSTAINML_CPP_DLL_API Node(
            const std::string& name,
            const Options& opts);

    SUSTAINML_CPP_DLL_API virtual ~Node();

    /**
     * @brief Called by the user to run the run.
     */
    SUSTAINML_CPP_DLL_API void spin();

    /**
     * @brief Stops the execution of the node.
     */
    SUSTAINML_CPP_DLL_API static void terminate();

    /**
     * @brief Retrieves the node name
     */
    SUSTAINML_CPP_DLL_API const std::string& name();

    /**
     * @brief Retrieves the node status
     */
    SUSTAINML_CPP_DLL_API const Status& status();

protected:

    /**
     * @brief Starts a new subscription (DataReader) in the
     * given topic.
     *
     * @param topic The topic name
     * @param type_name The type name
     * @param listener Listener object inheriting from DataReaderListener
     * @param opts Options to configure subscription QoS
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
     * @param opts Options to configure publication QoS
     */
    bool initialize_publication(
            const char* topic_name,
            const char* type_name,
            const Options& opts);

    /**
     * @brief Invokes the user callback with the provided inputs.
     *
     * @param inputs A vector containing the required samples. All the samples
     * must correspond to the same task_id.
     */
    virtual void publish_to_user(
            const int& task_id,
            const std::vector<std::pair<int, void*>> inputs) = 0;

    /**
     * @brief Publishes the internal status of the node to DDS.
     */
    void publish_node_status();

    /**
     * @brief Retrieves the node status
     */
    void status(
            const Status& status);

    /**
     * @brief Retrieves the inner writers
     */
    const std::vector<eprosima::fastdds::dds::DataWriter*>& writers();

private:

    /**
     * @brief Getter for the dispatcher
     *
     * @return A weak pointer to the Dispatcher object
     */
    std::weak_ptr<Dispatcher> get_dispatcher();

    //! Impl
    NodeImpl* impl_;

};

} // namespace core
} // namespace sustainml

#endif // SUSTAINMLCPP_CORE_NODE_HPP
