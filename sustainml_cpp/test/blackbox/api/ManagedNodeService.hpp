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
 * @file ManagedNodeService.hpp
 *
 */

#ifndef _TEST_BLACKBOX_MANAGEDNODESERVICE_HPP_
#define _TEST_BLACKBOX_MANAGEDNODESERVICE_HPP_

#include <future>

#include <sustainml_cpp/nodes/AppRequirementsNode.hpp>
#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>
#include <sustainml_cpp/nodes/HardwareConstraintsNode.hpp>
#include <sustainml_cpp/nodes/HardwareResourcesNode.hpp>
#include <sustainml_cpp/nodes/MLModelNode.hpp>
#include <sustainml_cpp/nodes/MLModelMetadataNode.hpp>
#include <sustainml_cpp/core/RequestReplyListener.hpp>

template <typename _NODE_TYPE, typename _LISTENER_TYPE, typename ... Args>
class ManagedNodeService
{
    using functor_t = std::function<void (Args&...)>;
    friend class ManagedNodeServiceListener;
    friend class ManagedRequestReplyListener;

    struct ManagedNodeServiceListener : public _LISTENER_TYPE
    {
        ManagedNodeServiceListener(
                ManagedNodeService* parent,
                const functor_t& functor = nullptr)
            : functor_(functor)
            , managed_node_(parent)
        {

        }

        virtual void on_new_task_available(
                Args&... args) override
        {
            std::cout << "New task available in " << managed_node_->node_.name() << std::endl;
            managed_node_->received_samples_.fetch_add(1);

            if (functor_)
            {
                functor_(std::forward<decltype(args)>(args)...);
            }

            managed_node_->cv_.notify_all();
        }

        std::function<void(Args&...)> functor_;
        ManagedNodeService* managed_node_;
    };

    struct ManagedRequestReplyListener : public sustainml::core::RequestReplyListener
    {
        ManagedRequestReplyListener(
                ManagedNodeService* parent)
            : managed_node_(parent)
        {

        }

        virtual void on_configuration_request(
                types::RequestType& req,
                types::ResponseType& res) override
        {
            std::cout << "New request available in " << req.node_id() << std::endl;

            res.node_id(req.node_id());
            res.transaction_id(req.transaction_id());
            res.success(true);
            res.configuration(req.configuration());

            managed_node_->cv_.notify_all();
            std::cout << "Sending back response" << req.node_id() << std::endl;
        }

        ManagedNodeService* managed_node_;
    };

public:

    ManagedNodeService(
            const functor_t& callback = nullptr)
        : listener_(this, callback)
        , req_listener_(this)
        , node_(listener_, req_listener_)
        , received_samples_(0)
        , expected_samples_(0)
    {

    }

    ~ManagedNodeService()
    {
        stop();
        th_->join();
    }

    void start()
    {
        std::packaged_task<void()> task ([&]()
                {
                    node_.spin();
                });

        th_.reset(new std::thread(std::move(task)));
    }

    void stop()
    {
        _NODE_TYPE::terminate();
    }

    void prepare_expected_samples(
            const size_t& expected_samples)
    {
        expected_samples_.store(expected_samples);
    }

    bool block_for_all()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() -> bool
                {
                    return expected_samples_ == received_samples_;
                });

        return expected_samples_ == received_samples_;
    }

    template<class _Rep,
            class _Period
            >
    size_t block_for_all(
            const std::chrono::duration<_Rep, _Period>& max_wait)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait_for(lock, max_wait, [this]() -> bool
                {
                    return expected_samples_ == received_samples_;
                });

        return expected_samples_ == received_samples_;
    }

private:

    ManagedNodeServiceListener listener_;
    ManagedRequestReplyListener req_listener_;
    _NODE_TYPE node_;

    std::mutex mtx_;
    std::condition_variable cv_;
    std::unique_ptr<std::thread> th_;

    std::atomic<size_t> received_samples_;
    std::atomic<size_t> expected_samples_;
};

#endif // _TEST_BLACKBOX_MANAGEDNODESERVICE_HPP_

