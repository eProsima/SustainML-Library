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
 * @file NodeListener.cpp
 */

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include <core/Dispatcher.hpp>
#include <core/NodeListener.hpp>

namespace sustainml {
namespace core {

template <typename T>
NodeListener<T>::NodeListener(
        Node* node,
        interfaces::QueueQueryable<T>* qq)
    : node_(node)
    , queue_queryable_(qq)
    , stop_ (false)
{

}

template <typename T>
NodeListener<T>::~NodeListener()
{
    stop();
}

template <typename T>
void NodeListener<T>::stop()
{
    stop_.store(true);
}

template <typename T>
void NodeListener<T>::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    eprosima::fastdds::dds::SampleInfo info;

    SamplesQueue<T>* queue = queue_queryable_->get_queue();

    T* data_cache = queue->get_new_cache();

    if (nullptr == data_cache)
    {
        EPROSIMA_LOG_ERROR(NODE_LISTENER, node_->name() << " Could not get a new cache. Queue is full");
        return;
    }

    while (!stop_.load(std::memory_order_relaxed))
    {
        if (reader->take_next_sample(data_cache->get_impl(),
                &info) == eprosima::fastdds::dds::RETCODE_OK)
        {
            if (info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE)
            {
                // Print your structure data here.
                EPROSIMA_LOG_INFO(NODE_LISTENER,
                        node_->name() << " Message with task_id: " << data_cache->task_id() << " in " << reader->guid() <<
                        " RECEIVED");
                queue->insert_element(data_cache);

                // notify dispatcher
                if (auto dispatcher = node_->get_dispatcher().lock())
                {
                    dispatcher->notify(data_cache->task_id());
                }

                //! We need to use a new one in case there are more samples
                data_cache = queue->get_new_cache();

                if (nullptr == data_cache)
                {
                    EPROSIMA_LOG_ERROR(NODE_LISTENER, node_->name() << " Could not get a new cache. Queue is full");
                    break;
                }
            }
        }
        else
        {
            queue->release_cache(data_cache);
            break;
        }
    }
}

template <typename T>
void NodeListener<T>::on_subscription_matched(
        eprosima::fastdds::dds::DataReader* reader,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus& status)
{
    if (status.current_count_change == 1)
    {
        EPROSIMA_LOG_INFO(NODE_LISTENER, "Subscriber matched [ " << iHandle2GUID(
                    status.last_publication_handle) << " ].");
    }
    else if (status.current_count_change == -1)
    {
        EPROSIMA_LOG_INFO(NODE_LISTENER, "Subscriber unmatched [ " << iHandle2GUID(
                    status.last_publication_handle) << " ].");
    }
    else
    {
        EPROSIMA_LOG_INFO(NODE_LISTENER, status.current_count_change
                << " is not a valid value for SubscriptionMatchedStatus current count change");
    }
}

} // namespace core
} // namespace sustainml
