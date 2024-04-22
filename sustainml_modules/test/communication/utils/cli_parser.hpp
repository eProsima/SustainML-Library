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

#ifndef _TEST_COMMUNICATION_UTILS_CLIEPARSER_HPP_
#define _TEST_COMMUNICATION_UTILS_CLIEPARSER_HPP_

#include <bitset>
#include <cstring>
#include <iostream>

#include "EnumTypes.hpp"

inline bool parse_cli_args(
        const int& argc,
        char** argv,
        bool& publisher,
        uint32_t& samples,
        std::string& topic_name,
        std::bitset<TopicType::MAX>& type_opts)
{
    bool ret = true;
    int arg_count = 1;

    for (int i = 0; i < argc - 1; i++)
    {
        printf("%s ", argv[i]);
    }

    while (arg_count < argc)
    {
        if (strcmp(argv[arg_count], "publisher") == 0)
        {
            publisher = true;
        }
        else if (strcmp(argv[arg_count], "subscriber") == 0)
        {
            publisher = false;
        }
        else if (strcmp(argv[arg_count], "--ui") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[USER_INPUT] = true;
        }
        else if (strcmp(argv[arg_count], "--mlm") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[ML_MODEL_METADATA] = true;
        }
        else if (strcmp(argv[arg_count], "--ml") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[ML_MODEL] = true;
        }
        else if (strcmp(argv[arg_count], "--hw") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[HW_RESOURCES] = true;
        }
        else if (strcmp(argv[arg_count], "--co2") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[CO2_FOOTPRINT] = true;
        }
        else if (strcmp(argv[arg_count], "--hwc") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[HW_CONSTRAINTS] = true;
        }
        else if (strcmp(argv[arg_count], "--app") == 0)
        {
            if (type_opts.any())
            {
                std::cout << "Cannot request more than one type" << std::endl;
                return -1;
            }
            type_opts[APP_REQUIREMENTS] = true;
        }
        else if (strcmp(argv[arg_count], "--samples") == 0)
        {
            if (++arg_count >= argc)
            {
                std::cout << "--samples expects a parameter" << std::endl;
                return -1;
            }

            samples = strtol(argv[arg_count], nullptr, 10);
        }
        else if (strcmp(argv[arg_count], "--topic") == 0)
        {
            if (++arg_count >= argc)
            {
                std::cout << "--topic expects a parameter" << std::endl;
                return -1;
            }

            topic_name = argv[arg_count];
        }
        else
        {
            std::cout << "Wrong argument " << argv[arg_count] << std::endl;
            ret = false;
            break;
        }

        ++arg_count;
    }

    if (ret && (type_opts.none() || topic_name.empty()))
    {
        std::cout << "At least one topic type must be defined" << std::endl;
        ret = false;
    }

    return ret;
}

#endif // _TEST_COMMUNICATION_UTILS_CLIEPARSER_HPP_