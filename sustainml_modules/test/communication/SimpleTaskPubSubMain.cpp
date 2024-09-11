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
#include "SimpleTaskSubscriber.hpp"

#include "types/typesImplPubSubTypes.hpp"

#include "utils/cli_parser.hpp"
#include "utils/TypeFactory.hpp"

int main(
        int argc,
        char** argv)
{
    bool publisher = false;
    uint32_t samples = 4;
    std::string topic_name;
    std::bitset<TopicType::MAX> type_opts;
    type_opts.reset();

    if (!parse_cli_args(argc, argv, publisher, samples, topic_name, type_opts))
    {
        std::cerr << "Error launching SimpleTaskPubSubMain" << std::endl;
    }
    else
    {
        auto type = TypeFactory::make_type(type_opts);

        if (publisher)
        {
            SimpleTaskPublisher publisher;

            if (publisher.init(samples, topic_name, type))
            {
                return publisher.run() ? 0 : -1;
            }
        }
        else
        {
            SimpleTaskSubscriber subscriber;

            if (subscriber.init(samples, topic_name, type))
            {
                return subscriber.run() ? 0 : -1;
            }
        }
    }

    return -1;
}
