#include <cstdlib>
#include <functional>
#include <csignal>

#include <sustainml_cpp/nodes/CarbonFootprintNode.hpp>

void cb(MLModel model, UserInput ui, HWResource hw, NodeStatus status, CO2Footprint output)
{
    std::cout << "User Received task IDs: " << model.task_id()  << " " << ui.task_id() << " " << hw.task_id()
    << " DATA -> " << model.task_id() << " " << ui.task_id() << " " << hw.task_id() << std::endl;
}

int main()
{

    signal(SIGINT, [](int signum)
            {
                std::cout << "SIGINT received, stopping execution." << std::endl;
                static_cast<void>(signum); sustainml::Node::terminate();
            });

    sustainml::co2_tracker::CarbonFootprintNode n;

    n.register_cb(&cb);

    n.spin();

    return EXIT_SUCCESS;
}
