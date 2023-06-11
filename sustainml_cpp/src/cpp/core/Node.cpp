#include <sustainml_cpp/core/Node.hpp>

namespace sustainml {

    std::atomic<bool> Node::terminate_(false);
    std::condition_variable Node::spin_cv_;

    void Node::spin()
    {

    }

    void Node::initialize_subscription(
        const char* topic,
        eprosima::fastdds::dds::DataReaderListener* listener)
    {

    }

    void Node::initialize_publication(
        const char* topic)
    {

    }

    void Node::terminate()
    {

    }

    void Node::publish_to_user(const std::vector<void*> inputs)
    {

    }

} // namespace sustainml
