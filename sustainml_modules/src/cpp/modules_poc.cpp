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
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/common/InstanceHandle.hpp>
#include <fastrtps/types/TypesBase.h>
#include <sustainml_cpp/nodes/TaskEncoderNode.hpp>
#include <sustainml_cpp/nodes/MLModelNode.hpp>
#include <sustainml_cpp/nodes/HardwareResourcesNode.hpp>
#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>
#include <sustainml_cpp/types/types.h>
#include <sustainml_cpp/types/typesPubSubTypes.h>

#define DEBUG_MODE true

namespace fdds = eprosima::fastdds::dds;
namespace frtps = eprosima::fastdds::rtps;
namespace ftypes = eprosima::fastrtps::types;

class WriterListener: public fdds::DataWriterListener {

public:

    void on_publication_matched(fdds::DataWriter *writer,
                                const fdds::PublicationMatchedStatus &info) override
    {
        if (info.current_count_change >= 1) {
            std::cout << "publication matched topic: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else if (info.current_count_change <= -1) {
            std::cout << "publication unmatched topic: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else {
            std::cout << "publication matched unchanged: " << writer->get_topic()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
    }
};

class ReaderListener: public fdds::DataReaderListener {

public:

    virtual void on_data_available(fdds::DataReader *reader) override
    {
        CO2Footprint co2_f;
        fdds::SampleInfo info;

        std::cout << "on_data_available" << std::endl;

        while (1) {
            auto ret = reader->take_next_sample(&co2_f, &info);
            if (ret == ReturnCode_t::RETCODE_NO_DATA) {
                break;
            }
            else if (ret != ReturnCode_t::RETCODE_OK) {
                std::cerr << "take_next_sample() failed!" << std::endl;
                break;
            }
            else {
                std::cout << "take_next_sample() success: instance_state["
                          << ((info.instance_state & fdds::ALIVE_INSTANCE_STATE) ? "Alive" :
                             ((info.instance_state & fdds::NOT_ALIVE_DISPOSED_INSTANCE_STATE) ? "Disposed" :
                              ((info.instance_state & fdds::NOT_ALIVE_NO_WRITERS_INSTANCE_STATE) ? "No writers" : "")))
                          << "]"
                          << std::endl;
                if (info.valid_data) {
                    std::cout << "carbon_intensity:\n\tid: " << co2_f.carbon_intensity()
                              << "\nco2_footprint: " << co2_f.co2_footprint()
                              << std::endl;
                }
                else {
                    break;
                }
            }
        }
    }

    virtual void on_subscription_matched(fdds::DataReader *reader,
                                         const fdds::SubscriptionMatchedStatus &info) override
    {
        if (info.current_count_change >= 1) {
            std::cout << "subscription matched on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else if (info.current_count_change <= -1) {
            std::cout << "subscription unmatched on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
        else {
            std::cout << "publication matched unchanged on topic: " << reader->get_topicdescription()->get_name()
                      << ", current_count: " << info.current_count
                      << ", current_count_change: " << info.current_count_change
                      << std::endl;
        }
    }
};

class UserInputPublisher {
public:

    UserInputPublisher()
    {
        participant = fdds::DomainParticipantFactory::get_instance()->create_participant(
                                0, eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
        publisher = participant->create_publisher(fdds::PUBLISHER_QOS_DEFAULT);
        type = static_cast<eprosima::fastdds::dds::TypeSupport>(new UserInputPubSubType);
        type.register_type(participant);
        topic = participant->create_topic("/sustainml/user_input", "UserInput", fdds::TOPIC_QOS_DEFAULT);
        listener = new WriterListener();
        writer = publisher->create_datawriter(topic, eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT, listener);
    }

    ~UserInputPublisher()
    {
        publisher->delete_datawriter(writer);
        participant->delete_publisher(publisher);
        participant->delete_topic(topic);
        fdds::DomainParticipantFactory::get_instance()->delete_participant(participant);
    }

    void send_sample(int task_id)
    {
        UserInput ui;
        GeoLocation geo;
        geo.continent("Continent of task id " + std::to_string(task_id));
        geo.region("Region of task id " + std::to_string(task_id));
        ui.geo_location(geo);
        ui.problem_description("Problem definition of task id " + std::to_string(task_id));
        ui.task_id(task_id);

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

class CO2FootprintSubscriber {
public:

    CO2FootprintSubscriber()
    {
        participant = fdds::DomainParticipantFactory::get_instance()->create_participant(
                                0, eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
        subscriber = participant->create_subscriber(fdds::SUBSCRIBER_QOS_DEFAULT);
        type = static_cast<eprosima::fastdds::dds::TypeSupport>(new CO2FootprintPubSubType);
        type.register_type(participant);
        topic = participant->create_topic("/sustainml/carbon_tracker/output", "CO2Footprint", fdds::TOPIC_QOS_DEFAULT);
        listener = new ReaderListener();
        reader = subscriber->create_datareader(topic, eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT, listener);

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

int main()
{
    // Register the nodes
    sustainml::ml_task_encoding_module::TaskEncoderNode ml_task_encoding_node_mock;
    sustainml::ml_model_provider_module::MLModelNode ml_model_provider_node_mock;
    sustainml::hardware_module::HardwareResourcesNode hardware_node_mock;
    sustainml::co2_tracker_module::CarbonFootprintNode co2_tracker_node_mock;

    // Register user input publisher, which would trigger the modules in cascade
    UserInputPublisher user_input_publisher;
    // Register final user data subscriber, which would print Carbon footprint output
    CO2FootprintSubscriber co2_footprint_subscriber;

    // Register SIGINT signal handler to stop app execution and all nodes
    signal(SIGINT, [](int signum)
    {
        std::cout << "SIGINT received, stopping execution." << std::endl;
        static_cast<void>(signum);
        sustainml::ml_task_encoding_module::TaskEncoderNode::terminate();
        sustainml::ml_model_provider_module::MLModelNode::terminate();
        sustainml::hardware_module::HardwareResourcesNode::terminate();
        sustainml::co2_tracker_module::CarbonFootprintNode::terminate();
    });

    // Assign each node their main method callback
    ml_task_encoding_node_mock.register_cb([] (
        UserInput& user_input,
        NodeStatus& status,
        EncodedTask& output)
        {
            // Set up node
            status.update(Status::NODE_INITIALIZING);

            // Print task id input
            if (DEBUG_MODE)
            {
                std::cout << "ML task encoding received task ID: " << std::endl;
                std::cout << " User input task: " << user_input.task_id() << std::endl;
            }

            // Wait the time it takes the node to initialize
            sleep(1);

            // Update the status to running
            status.update(Status::NODE_RUNNING);

            // Populate output
            output.keywords(std::vector<std::string>
                    {"keywords", "from", "task", std::to_string(user_input.task_id())});

            // Wait the time it takes the node to generate the output
            sleep(1);

            // Print node output
            if (DEBUG_MODE)
            {
                std::cout << "ML task encoding output generated: " << std::endl;
                std::cout << " Keywords: ";
                int count = 0;
                for (std::vector<std::string>::iterator it = output.keywords().begin(); it != output.keywords().end(); ++it, count++)
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
    );
    ml_model_provider_node_mock.register_cb([] (
        EncodedTask& encoded_task,
        NodeStatus& status,
        MLModel& output)
        {
            // Set up node
            status.update(Status::NODE_INITIALIZING);

            // Print task id input
            if (DEBUG_MODE)
            {
                std::cout << "ML model provider received task ID: " << std::endl;
                std::cout << " ML encoded task: " << encoded_task.task_id() << std::endl;
            }

            // Wait the time it takes the node to initialize
            sleep(1);

            // Update the status to running
            status.update(Status::NODE_RUNNING);

            // Populate output
            output.model("ML model #" + std::to_string(encoded_task.task_id())
                    + " ONNX would go here, parsed to string");
            output.model_path("/opt/sustainml/ml_model/" + std::to_string(encoded_task.task_id())
                    + "/model.onnx");
            output.model_properties("ML model #" + std::to_string(encoded_task.task_id())
                    + " properties would go here, parsed to string");
            output.model_path("/opt/sustainml/ml_model/" + std::to_string(encoded_task.task_id())
                    + "/properties.json");

            // Wait the time it takes the node to generate the output
            sleep(6);

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
    );
    hardware_node_mock.register_cb([] (
        MLModel& model,
        NodeStatus& status,
        HWResource& output)
        {
            // Set up node
            status.update(Status::NODE_INITIALIZING);

            // Print task id input
            if (DEBUG_MODE)
            {
                std::cout << "HW resource received task ID: " << std::endl;
                std::cout << " ML model provider: " << model.task_id()  << std::endl;
            }

            // Wait the time it takes the node to initialize
            sleep(1);

            // Update the status to running
            status.update(Status::NODE_RUNNING);

            // Populate output
            output.hw_description("HW descr. of task #" + std::to_string(model.task_id()));
            output.power_consumption(model.task_id()+1000.3);

            // Wait the time it takes the node to generate the output
            sleep(2);

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
    );
    co2_tracker_node_mock.register_cb([] (
        MLModel& model,
        UserInput& user_input,
        HWResource& hardware_resources,
        NodeStatus& status,
        CO2Footprint& output)
        {
            // Set up node
            status.update(Status::NODE_INITIALIZING);

            // Print task id input
            std::cout << "CO2 Footprint received task IDs: " << std::endl;
            std::cout << " ML model provider: " << model.task_id() << std::endl;
            std::cout << " User input:        " << user_input.task_id() << std::endl;
            std::cout << " HW resource:       " << hardware_resources.task_id() << std::endl;

            // Wait the time it takes the node to initialize
            sleep(1);

            // Update the status to running
            status.update(Status::NODE_RUNNING);

            // Populate output
            output.carbon_intensity  (model.task_id()+0.1);
            output.co2_footprint     (model.task_id()+100.2);
            output.energy_consumption(model.task_id()+1000.3);

            // Wait the time it takes the node to generate the output
            sleep(4);

            // Print node output
            std::cout << "CO2 Footprint output generated: " << std::endl;
            std::cout << " carbon intensity:   " << std::to_string(output.carbon_intensity()) << std::endl;
            std::cout << " co2 footprint:      " << std::to_string(output.co2_footprint()) << std::endl;
            std::cout << " energy consumption: " << std::to_string(output.energy_consumption()) << std::endl;

            // Update the status when finished
            status.update(Status::NODE_IDLE);
        }
    );

    // Spin all nodes
    auto p_task_encoding = std::async([&ml_task_encoding_node_mock](){ ml_task_encoding_node_mock.spin(); });
    auto p_ml_model_provider = std::async([&ml_model_provider_node_mock](){ ml_model_provider_node_mock.spin(); });
    auto p_hardware = std::async([&hardware_node_mock](){ hardware_node_mock.spin(); });
    auto p_carbon_tracker = std::async([&co2_tracker_node_mock](){ co2_tracker_node_mock.spin(); });

    // Wait for spin
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Input loop
    uint16_t task_id = 1;
    char input = 0;

    do {
        std::cout << "<--- Press a key and enter to continue, or q/Q plus enter to exit: ";
        std::cin >> input;

        if((input == 'q') || (input == 'Q'))
        {
            break;
        }

        // Publish sample
        user_input_publisher.send_sample(task_id);

        task_id++;

    } while (1);

    sustainml::ml_task_encoding_module::TaskEncoderNode::terminate();
    sustainml::ml_model_provider_module::MLModelNode::terminate();
    sustainml::hardware_module::HardwareResourcesNode::terminate();
    sustainml::co2_tracker_module::CarbonFootprintNode::terminate();

    p_task_encoding.get();
    p_ml_model_provider.get();
    p_hardware.get();
    p_carbon_tracker.get();

    std::cout << "EXITING !!" << std::endl;

    return EXIT_SUCCESS;
}
