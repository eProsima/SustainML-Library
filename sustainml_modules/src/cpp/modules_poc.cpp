#include <csignal>
#include <cstdlib>
#include <functional>
#include <unistd.h>

#include <sustainml_cpp/nodes/TaskEncoderNode.hpp>
#include <sustainml_cpp/nodes/MLModelNode.hpp>
#include <sustainml_cpp/nodes/HardwareResourcesNode.hpp>
#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>

#include "PubSubWriterReader.hpp"

#define DEBUG_MODE false

std::list<UserInput> user_input_data_generator(
        uint16_t task_id)
{
    std::list<UserInput> returnedValue(1);

    std::generate(returnedValue.begin(), returnedValue.end(), [&task_id]
            {
                UserInput ui;
                GeoLocation geo;
                geo.continent("Continent of task id " + std::to_string(task_id));
                geo.region("Region of task id " + std::to_string(task_id));
                ui.geo_location(geo);
                ui.problem_description("Problem definition of task id " + std::to_string(task_id));
                ui.task_id(task_id);
                return ui;
            });

    return returnedValue;
}

int main()
{
    // Register the nodes
    sustainml::ml_task_encoding_module::TaskEncoderNode ml_task_encoding_node_mock;
    sustainml::ml_model_provider_module::MLModelNode ml_model_provider_node_mock;
    sustainml::hardware_module::HardwareResourcesNode hardware_node_mock;
    sustainml::co2_tracker_module::CarbonFootprintNode co2_tracker_node_mock;

    // Register writer for user input and readers for each output
    PubSubWriterReader<UserInput> ui_writer("UserInput");
    PubSubWriterReader<EncodedTask> et_reader("EncodedTask");
    PubSubWriterReader<MLModel> ml_reader("MLModel");
    PubSubWriterReader<HWResource> hw_reader("HWResource");
    PubSubWriterReader<CO2Footprint> co_reader("CO2Footprint");
    ui_writer.init();
    et_reader.init();
    ml_reader.init();
    hw_reader.init();
    co_reader.init();

    // Wait for discovery.
    ui_writer.wait_discovery();
    et_reader.wait_discovery();
    ml_reader.wait_discovery();
    hw_reader.wait_discovery();
    co_reader.wait_discovery();

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
        UserInput user_input,
        NodeStatus status,
        EncodedTask output)
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
        EncodedTask encoded_task,
        NodeStatus status,
        MLModel output)
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
        MLModel model,
        NodeStatus status,
        HWResource output)
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
        MLModel model,
        UserInput user_input,
        HWResource hardware_resources,
        NodeStatus status,
        CO2Footprint output)
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
    ml_task_encoding_node_mock.spin();
    ml_model_provider_node_mock.spin();
    hardware_node_mock.spin();
    co2_tracker_node_mock.spin();

    // Block reader until reception finished or timeout.
    et_reader.block_for_all();
    ml_reader.block_for_all();
    hw_reader.block_for_all();
    co_reader.block_for_all();

    // Wait until user press enter
    uint16_t task_id = 1;
    do
    {
        std::cout << "\nPress a key to continue..." << std::endl;

        // Create UserInput sample
        auto data = user_input_data_generator(task_id);
        ++task_id;
        // Publish sample
        ui_writer.send(data);
    }
    while (std::cin.get() != '\n');

    return EXIT_SUCCESS;
}
