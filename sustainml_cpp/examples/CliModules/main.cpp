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
 * @file main.cpp
 */

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/common/InstanceHandle.hpp>

#include <sustainml_cpp/nodes/AppRequirementsNode.hpp>
#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>
#include <sustainml_cpp/nodes/HardwareConstraintsNode.hpp>
#include <sustainml_cpp/nodes/HardwareResourcesNode.hpp>
#include <sustainml_cpp/nodes/MLModelMetadataNode.hpp>
#include <sustainml_cpp/nodes/MLModelNode.hpp>

#include "typesImplPubSubTypes.hpp"

bool DEBUG_MODE  = false;

namespace fdds = eprosima::fastdds::dds;

class WriterListener : public fdds::DataWriterListener
{

public:

    void on_publication_matched(
            fdds::DataWriter* writer,
            const fdds::PublicationMatchedStatus& info) override
    {
        if (info.current_count_change >= 1)
        {
            std::cout << "publication matched topic: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else if (info.current_count_change <= -1)
        {
            std::cout << "publication unmatched topic: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else
        {
            std::cout << "publication matched unchanged: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
    }

};

class ReaderListener : public fdds::DataReaderListener
{

public:

    virtual void on_data_available(
            fdds::DataReader* reader) override
    {
        CO2FootprintImpl co2_f;
        fdds::SampleInfo info;

        if (DEBUG_MODE)
        {
            std::cout << "on_data_available" << std::endl;
        }

        while (1)
        {
            auto ret = reader->take_next_sample(&co2_f, &info);
            if (ret == eprosima::fastdds::dds::RETCODE_NO_DATA)
            {
                break;
            }
            else if (ret != eprosima::fastdds::dds::RETCODE_OK)
            {
                std::cerr << "take_next_sample() failed!" << std::endl;
                break;
            }
            else
            {
                if (DEBUG_MODE)
                {
                    std::cout << "take_next_sample() success: instance_state["
                              << ((info.instance_state & fdds::ALIVE_INSTANCE_STATE) ? "Alive" :
                    ((info.instance_state & fdds::NOT_ALIVE_DISPOSED_INSTANCE_STATE) ? "Disposed" :
                    ((info.instance_state & fdds::NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) ? "No writers" : "")))
                              << "]"
                              << std::endl;
                    if (info.valid_data)
                    {
                        std::cout << "carbon_intensity:\n\tid: " << co2_f.carbon_intensity()
                                  << "\ncarbon_footprint: " << co2_f.carbon_footprint()
                                  << std::endl;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    virtual void on_subscription_matched(
            fdds::DataReader* reader,
            const fdds::SubscriptionMatchedStatus& info) override
    {
        if (info.current_count_change >= 1)
        {
            std::cout << "subscription matched on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else if (info.current_count_change <= -1)
        {
            std::cout << "subscription unmatched on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else
        {
            std::cout << "publication matched unchanged on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
    }

};

class UserInputPublisher
{
public:

    UserInputPublisher()
    {
        participant = fdds::DomainParticipantFactory::get_instance()->create_participant(
            0, eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
        publisher = participant->create_publisher(fdds::PUBLISHER_QOS_DEFAULT);
        type = static_cast<eprosima::fastdds::dds::TypeSupport>(new UserInputImplPubSubType);
        type.register_type(participant);
        topic = participant->create_topic("/sustainml/user_input", "UserInputImpl", fdds::TOPIC_QOS_DEFAULT);
        listener = new WriterListener();
        fdds::DataWriterQos wqos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;
        wqos.resource_limits().max_instances = 500;
        wqos.resource_limits().max_samples_per_instance = 1;
        writer = publisher->create_datawriter(topic, wqos, listener);
    }

    ~UserInputPublisher()
    {
        publisher->delete_datawriter(writer);
        participant->delete_publisher(publisher);
        participant->delete_topic(topic);
        fdds::DomainParticipantFactory::get_instance()->delete_participant(participant);
    }

    void send_sample(
            int task_id)
    {
        UserInputImpl ui;
        TaskIdImpl task_id_impl;
        task_id_impl.problem_id() = task_id;
        task_id_impl.iteration_id() = 1;
        ui.geo_location_continent("Continent of task id " + std::to_string(task_id));
        ui.geo_location_region("Region of task id " + std::to_string(task_id));
        ui.problem_definition("Problem definition of task id " + std::to_string(task_id));
        ui.task_id(task_id_impl);

        // publish new sample
        writer->write(&ui);
    }

private:

    fdds::DomainParticipant* participant;
    fdds::Publisher* publisher;
    fdds::DataWriter* writer;
    fdds::TypeSupport type;
    fdds::Topic* topic;
    WriterListener* listener;
};

class CO2FootprintSubscriber
{
public:

    CO2FootprintSubscriber()
    {
        participant = fdds::DomainParticipantFactory::get_instance()->create_participant(
            0, eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
        subscriber = participant->create_subscriber(fdds::SUBSCRIBER_QOS_DEFAULT);
        type = static_cast<eprosima::fastdds::dds::TypeSupport>(new CO2FootprintImplPubSubType);
        type.register_type(participant);
        topic = participant->create_topic("/sustainml/carbon_tracker/output", "CO2FootprintImpl",
                        fdds::TOPIC_QOS_DEFAULT);
        listener = new ReaderListener();
        fdds::DataReaderQos rqos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
        rqos.resource_limits().max_instances = 500;
        rqos.resource_limits().max_samples_per_instance = 1;
        reader = subscriber->create_datareader(topic, rqos, listener);

    }

    ~CO2FootprintSubscriber()
    {
        subscriber->delete_datareader(reader);
        participant->delete_subscriber(subscriber);
        participant->delete_topic(topic);
        fdds::DomainParticipantFactory::get_instance()->delete_participant(participant);
    }

private:

    fdds::DomainParticipant* participant;
    fdds::Subscriber* subscriber;
    fdds::DataReader* reader;
    fdds::TypeSupport type;
    fdds::Topic* topic;
    ReaderListener* listener;
};

void print_usage()
{
    std::cout << "SustainML modules mock requires the following arguments:" << std::endl;
    std::cout << "./sustainml_modules_poc <module> <print_mode>" << std::endl;
    std::cout << "    <module> options are:" << std::endl;
    std::cout << "        all:          all modules are executed in the same process." << std::endl;
    std::cout << "        carbon:       execute a carbon footprint provider node." << std::endl;
    std::cout << "        constraints:  execute a hardware constraints node." << std::endl;
    std::cout << "        input:        execute a user input node." << std::endl;
    std::cout << "        metadata:     execute a ML model metadata node." << std::endl;
    std::cout << "        model:        execute a ML model provider node." << std::endl;
    std::cout << "        requirements: execute a application requirements node." << std::endl;
    std::cout << "        resources:    execute an hardware resources provider node." << std::endl;
    std::cout << "    <print_mode> options are:" << std::endl;
    std::cout << "        true:     print extended information of the selected node(s)." << std::endl;
    std::cout << "        false:    print minimal information of the selected node(s)" << std::endl;
}

class CustomAppRequirementsListener : public sustainml::app_requirements_module::AppRequirementsTaskListener
{

    void on_new_task_available(
            types::UserInput& user_input,
            types::NodeStatus& status,
            types::AppRequirements& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        if (DEBUG_MODE)
        {
            std::cout << "App requirements received task ID: " << std::endl;
            std::cout << " User input task: " << user_input.task_id() << std::endl;
        }

        // Wait the time it takes the node to initialize
        sleep(1);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.app_requirements(std::vector<std::string>
                {"requirements", "from", "task", std::to_string(user_input.task_id().problem_id()),
                 std::to_string(user_input.task_id().iteration_id())});

        // Wait the time it takes the node to generate the output
        sleep(1);
        // Print node output
        if (DEBUG_MODE)
        {
            std::cout << "App requirements output generated: " << std::endl;
            std::cout << " Requirements: ";
            int count = 0;
            for (std::vector<std::string>::iterator it = output.app_requirements().begin();
                    it != output.app_requirements().end();
                    ++it, count++)
            {
                if (count != 0)
                {
                    std::cout << ", ";
                }
                std::cout << "'" << *it << "'";
            }
            std::cout << std::endl;
        }

        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

class CustomCarbonFootprintListener : public sustainml::carbon_tracker_module::CarbonFootprintTaskListener
{

    void on_new_task_available (
            types::MLModel& model,
            types::UserInput& user_input,
            types::HWResource& hardware_resources,
            types::NodeStatus& status,
            types::CO2Footprint& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        std::cout << "CO2 Footprint received task IDs: " << std::endl;
        std::cout << " ML model provider: " << model.task_id() << std::endl;
        std::cout << " User input:        " << user_input.task_id() << std::endl;
        std::cout << " HW resource:       " << hardware_resources.task_id() << std::endl;
        std::cout << " HW description:       " << hardware_resources.hw_description() << std::endl;

        // Wait the time it takes the node to initialize
        sleep(4);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.carbon_intensity(model.task_id().problem_id() + 0.1);
        output.carbon_footprint(model.task_id().problem_id() + 100.2);
        output.energy_consumption(model.task_id().problem_id() + 1000.3);

        // Wait the time it takes the node to generate the output
        sleep(3);

        // Print node output
        std::cout << "CO2 Footprint output generated: " << std::endl;
        std::cout << " carbon intensity:   " << std::to_string(output.carbon_intensity()) << std::endl;
        std::cout << " co2 footprint:      " << std::to_string(output.carbon_footprint()) << std::endl;
        std::cout << " energy consumption: " << std::to_string(output.energy_consumption()) << std::endl;

        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

class CustomHardwareConstraintsListener : public sustainml::hardware_module::HardwareConstraintsTaskListener
{

    void on_new_task_available(
            types::UserInput& user_input,
            types::NodeStatus& status,
            types::HWConstraints& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        if (DEBUG_MODE)
        {
            std::cout << "Hardware constraints received task ID: " << std::endl;
            std::cout << " User input task: " << user_input.task_id() << std::endl;
        }

        // Wait the time it takes the node to initialize
        sleep(1);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.max_memory_footprint(user_input.task_id().problem_id());

        // Wait the time it takes the node to generate the output
        sleep(1);
        // Print node output
        if (DEBUG_MODE)
        {
            std::cout << "Hardware constraints output generated: " << std::endl;
            std::cout << " Max memory footprint: " << output.max_memory_footprint() << std::endl;
        }

        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

class CustomHardwareResourcesListener : public sustainml::hardware_module::HardwareResourcesTaskListener
{

    void on_new_task_available (
            types::MLModel& model,
            types::AppRequirements& requirements,
            types::HWConstraints& constraints,
            types::NodeStatus& status,
            types::HWResource& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        if (DEBUG_MODE)
        {
            std::cout << "HW resource received task ID: " << std::endl;
            std::cout << " ML model provider:    " << model.task_id()  << std::endl;
            std::cout << " App requirements:     " << requirements.task_id()  << std::endl;
            std::cout << " Hardware constraints: " << constraints.task_id()  << std::endl;
        }

        // Wait the time it takes the node to initialize
        sleep(3);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.hw_description("HW descr. of task #" + std::to_string(
                    model.task_id().problem_id()) + " " + std::to_string(model.task_id().problem_id()));
        output.power_consumption(model.task_id().problem_id() + 1000.3);

        // Wait the time it takes the node to generate the output
        sleep(3);

        // Print node output
        if (DEBUG_MODE)
        {
            std::cout << "HW resource output generated: " << std::endl;
            std::cout << " hardware description: " << output.hw_description() << std::endl;
            std::cout << " power consumption:    " << std::to_string(output.power_consumption()) << std::endl;
        }
        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

class CustomMLModelMetadataListener : public sustainml::ml_model_module::MLModelMetadataTaskListener
{

    void on_new_task_available(
            types::UserInput& user_input,
            types::NodeStatus& status,
            types::MLModelMetadata& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        if (DEBUG_MODE)
        {
            std::cout << "ML model metadata received task ID: " << std::endl;
            std::cout << " User input task: " << user_input.task_id() << std::endl;
        }

        // Wait the time it takes the node to initialize
        sleep(1);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.keywords(std::vector<std::string>
                {"keywords", "from", "task", std::to_string(user_input.task_id().problem_id()),
                 std::to_string(user_input.task_id().problem_id())});

        // Wait the time it takes the node to generate the output
        sleep(1);
        // Print node output
        if (DEBUG_MODE)
        {
            std::cout << "ML model metadata output generated: " << std::endl;
            std::cout << " Keywords: ";
            int count = 0;
            for (std::vector<std::string>::iterator it = output.keywords().begin(); it != output.keywords().end();
                    ++it, count++)
            {
                if (count != 0)
                {
                    std::cout << ", ";
                }
                std::cout << "'" << *it << "'";
            }
            std::cout << std::endl;
        }

        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

class CustomMLModelListener : public sustainml::ml_model_module::MLModelTaskListener
{
    void on_new_task_available (
            types::MLModelMetadata& model_metadata,
            types::AppRequirements& app_requirements,
            types::HWConstraints& hw_constraints,
            types::MLModel& ml_model_baseline,
            types::HWResource& hw_baseline,
            types::CO2Footprint& carbonf_baseline,
            types::NodeStatus& status,
            types::MLModel& output) override
    {
        // Set up node
        status.update(Status::NODE_INITIALIZING);

        // Print task id input
        if (DEBUG_MODE)
        {
            std::cout << "ML model provider received task ID: " << std::endl;
            std::cout << " ML model metadata:    " << model_metadata.task_id() << std::endl;
            std::cout << " App requirements:     " << app_requirements.task_id() << std::endl;
            std::cout << " Hardware constraints: " << hw_constraints.task_id() << std::endl;
            std::cout << " ML model metadata keywords size:    " << model_metadata.keywords().size() << std::endl;
            std::cout << " App constraints size:               " << app_requirements.app_requirements().size() <<
                std::endl;
            std::cout << " Hardware constraints max footprint: " << hw_constraints.max_memory_footprint() << std::endl;
        }

        // Wait the time it takes the node to initialize
        sleep(2);

        // Update the status to running
        status.update(Status::NODE_RUNNING);

        // Populate output
        output.model("ML model #" + std::to_string(model_metadata.task_id().problem_id()) + " " +
                std::to_string(model_metadata.task_id().iteration_id())
                + " ONNX would go here, parsed to string");
        output.model_path("/opt/sustainml/ml_model/" + std::to_string(
                    model_metadata.task_id().problem_id()) + " " +
                std::to_string(model_metadata.task_id().iteration_id())
                + "/model.onnx");
        output.model_properties("ML model #" + std::to_string(
                    model_metadata.task_id().problem_id()) + " " +
                std::to_string(model_metadata.task_id().iteration_id())
                + " properties would go here, parsed to string");
        output.model_path("/opt/sustainml/ml_model/" + std::to_string(
                    model_metadata.task_id().problem_id()) + " " +
                std::to_string(model_metadata.task_id().iteration_id())
                + "/properties.json");

        // Wait the time it takes the node to generate the output
        usleep(2);

        // Print node output
        if (DEBUG_MODE)
        {
            std::cout << "ML model provider output generated: " << std::endl;
            std::cout << " ML model ONNX:            " << output.model() << std::endl;
            std::cout << " ML model path:            " << output.model_path() << std::endl;
            std::cout << " ML model properties:      " << output.model_properties() << std::endl;
            std::cout << " ML model properties path: " << output.model_properties_path() << std::endl;
        }

        // Update the status when finished
        status.update(Status::NODE_IDLE);
    }

};

int main (
        int argc,
        char** argv)
{
    if (argc != 3)
    {
        print_usage();
        return EXIT_FAILURE;
    }
    else if (!(std::strcmp(argv[1], "all") == 0 || std::strcmp(argv[1], "carbon") == 0
            || std::strcmp(argv[1], "constraints") == 0 || std::strcmp(argv[1], "input") == 0
            || std::strcmp(argv[1], "metadata") == 0 || std::strcmp(argv[1], "model") == 0
            || std::strcmp(argv[1], "requirements") == 0 || std::strcmp(argv[1], "resources") == 0)
            || !(std::strcmp(argv[2], "true") == 0 || std::strcmp(argv[2], "false") == 0))
    {
        print_usage();
        return EXIT_FAILURE;
    }

    // check print mode
    if (std::strcmp(argv[2], "true") == 0)
    {
        DEBUG_MODE = true;
    }

    // Register SIGINT signal handler to stop app execution and all nodes
    signal(SIGINT, [](int signum)
            {
                std::cout << "SIGINT received, stopping execution." << std::endl;
                static_cast<void>(signum);
                sustainml::app_requirements_module::AppRequirementsNode::terminate();
                sustainml::carbon_tracker_module::CarbonFootprintNode::terminate();
                sustainml::hardware_module::HardwareResourcesNode::terminate();
                sustainml::hardware_module::HardwareConstraintsNode::terminate();
                sustainml::ml_model_module::MLModelMetadataNode::terminate();
                sustainml::ml_model_module::MLModelNode::terminate();
            });

    // All nodes workflow
    if (std::strcmp(argv[1], "all") == 0)
    {
        // Register the nodes
        UserInputPublisher user_input_publisher;

        CustomAppRequirementsListener app_requirements_listener;
        sustainml::app_requirements_module::AppRequirementsNode app_requirements_node_mock(app_requirements_listener);

        CustomCarbonFootprintListener carbon_tracker_listener;
        sustainml::carbon_tracker_module::CarbonFootprintNode carbon_tracker_node_mock(carbon_tracker_listener);
        CO2FootprintSubscriber carbon_footprint_subscriber;

        CustomHardwareConstraintsListener hardware_constraints_listener;
        sustainml::hardware_module::HardwareConstraintsNode hardware_constraints_node_mock(
            hardware_constraints_listener);

        CustomHardwareResourcesListener hardware_resources_listener;
        sustainml::hardware_module::HardwareResourcesNode hardware_resources_node_mock(hardware_resources_listener);

        CustomMLModelMetadataListener ml_model_metadata_listener;
        sustainml::ml_model_module::MLModelMetadataNode ml_model_metadata_node_mock(ml_model_metadata_listener);

        CustomMLModelListener ml_model_provider_listener;
        sustainml::ml_model_module::MLModelNode ml_model_provider_node_mock(ml_model_provider_listener);

        // Spin nodes
        auto p_app_requirements = std::async([&app_requirements_node_mock]()
                        {
                            app_requirements_node_mock.spin();
                        });
        auto p_carbon_tracker = std::async([&carbon_tracker_node_mock]()
                        {
                            carbon_tracker_node_mock.spin();
                        });
        auto p_hardware_constraints = std::async([&hardware_constraints_node_mock]()
                        {
                            hardware_constraints_node_mock.spin();
                        });
        auto p_hardware_resources = std::async([&hardware_resources_node_mock]()
                        {
                            hardware_resources_node_mock.spin();
                        });
        auto p_model_metadata = std::async([&ml_model_metadata_node_mock]()
                        {
                            ml_model_metadata_node_mock.spin();
                        });
        auto p_ml_model_provider = std::async([&ml_model_provider_node_mock]()
                        {
                            ml_model_provider_node_mock.spin();
                        });

        // Wait for spin
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Input loop
        uint16_t task_id = 1;
        char input = 0;

        do {
            std::cout << "<--- Press a key and enter to continue, or q/Q plus enter to exit: " << std::endl;
            std::cin >> input;

            if ((input == 'q') || (input == 'Q'))
            {
                break;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Publish sample
            user_input_publisher.send_sample(task_id);

            task_id++;

            // Wait the time stdout
            sleep(20);

        } while (1);

        sustainml::app_requirements_module::AppRequirementsNode::terminate();
        sustainml::carbon_tracker_module::CarbonFootprintNode::terminate();
        sustainml::hardware_module::HardwareResourcesNode::terminate();
        sustainml::hardware_module::HardwareConstraintsNode::terminate();
        sustainml::ml_model_module::MLModelMetadataNode::terminate();
        sustainml::ml_model_module::MLModelNode::terminate();

        p_app_requirements.get();
        p_carbon_tracker.get();
        p_hardware_constraints.get();
        p_hardware_resources.get();
        p_ml_model_provider.get();
        p_model_metadata.get();
    }
    else if (std::strcmp(argv[1], "input") == 0)
    {
        // Input loop
        uint16_t task_id = 1;
        char input = 0;
        UserInputPublisher user_input_publisher;

        do {
            std::cout << "<--- Press a key and enter to continue, or q/Q plus enter to exit: " << std::endl;
            std::cin >> input;

            if ((input == 'q') || (input == 'Q'))
            {
                break;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Publish sample
            user_input_publisher.send_sample(task_id);

            task_id++;

            // Wait the time stdout
            usleep(500000);

        } while (1);
    }


    // Assign each node their main method callback
    if (std::strcmp(argv[1], "metadata") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomMLModelMetadataListener ml_model_metadata_listener;
        sustainml::ml_model_module::MLModelMetadataNode ml_model_metadata_node_mock(ml_model_metadata_listener);
        ml_model_metadata_node_mock.spin();
    }
    if (std::strcmp(argv[1], "model") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomMLModelListener ml_model_provider_listener;
        sustainml::ml_model_module::MLModelNode ml_model_provider_node_mock(ml_model_provider_listener);
        ml_model_provider_node_mock.spin();
    }
    if (std::strcmp(argv[1], "resources") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomHardwareResourcesListener hardware_resources_listener;
        sustainml::hardware_module::HardwareResourcesNode hardware_resources_node_mock(hardware_resources_listener);
        hardware_resources_node_mock.spin();
    }
    if (std::strcmp(argv[1], "carbon") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomCarbonFootprintListener carbon_tracker_listener;
        sustainml::carbon_tracker_module::CarbonFootprintNode carbon_tracker_node_mock(carbon_tracker_listener);
        carbon_tracker_node_mock.spin();
    }
    if (std::strcmp(argv[1], "requirements") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomAppRequirementsListener app_requirements_listener;
        sustainml::app_requirements_module::AppRequirementsNode app_requirements_node_mock(app_requirements_listener);
        app_requirements_node_mock.spin();
    }
    if (std::strcmp(argv[1], "constraints") == 0)
    {
        std::cout << "<--- Press a CTRL + C to exit: " << std::endl;
        CustomHardwareConstraintsListener hardware_constraints_listener;
        sustainml::hardware_module::HardwareConstraintsNode hardware_constraints_node_mock(
            hardware_constraints_listener);
        hardware_constraints_node_mock.spin();
    }
    return EXIT_SUCCESS;
}
