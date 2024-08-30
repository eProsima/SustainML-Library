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

#include "SimpleTaskPublisher.hpp"

#include "utils/TypeFactory.hpp"

SimpleTaskPublisher::SimpleTaskPublisher()
    : samples_(0)
    , publisher_(nullptr)
    , datawriter_(nullptr)
    , matched_(false)
    , listener_(this)
{

}

SimpleTaskPublisher::~SimpleTaskPublisher()
{
    if (datawriter_ != nullptr)
    {
        publisher_->delete_datawriter(datawriter_);
    }
    if (publisher_ != nullptr)
    {
        participant_->delete_publisher(publisher_);
    }
}

bool SimpleTaskPublisher::init(
        uint32_t& samples,
        std::string& topic_name,
        eprosima::fastdds::dds::TypeSupport& ts)
{
    SimpleTaskBase::init(samples, topic_name, ts);

    samples_ = samples;

    //CREATE THE PUBLISHER
    eprosima::fastdds::dds::PublisherQos pubqos =
            eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT;

    publisher_ = participant_->create_publisher(
        pubqos,
        nullptr);

    if (publisher_ == nullptr)
    {
        return false;
    }

    // CREATE THE WRITER
    eprosima::fastdds::dds::DataWriterQos wqos =
            eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;

    wqos.resource_limits().max_instances = 500;
    wqos.resource_limits().max_samples_per_instance = 1;

    datawriter_ = publisher_->create_datawriter(
        topic_,
        wqos,
        &listener_);

    if (datawriter_ == nullptr)
    {
        return false;
    }

    return true;
}

void SimpleTaskPublisher::wait_discovery()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(2), [&]()
            {
                return matched_;
            });
}

bool SimpleTaskPublisher::run()
{
    auto data = type_.create_data();
    size_t task_id = 1;

    std::cout << "Waiting for discovery " << type_.get_type_name() << std::endl;
    wait_discovery();
    std::cout << "Discovery finished " << type_.get_type_name() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    TypeFactory::set_data_task_id(type_, data, task_id);

    if (type_.get_type_name() == "UserInputImpl")
    {
        auto ui_data = static_cast<UserInputImpl*>(data);
        ui_data->modality("video");
        ui_data->problem_definition("Classify cars in a video sequence.");
    }

    while (samples_ + 1 > task_id)
    {
        std::cout << "Publishing " << type_.get_type_name() << " with Task_id " << task_id << std::endl;

        datawriter_->write(data);

        ++task_id;

        TypeFactory::set_data_task_id(type_, data, task_id);

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return true;
}
