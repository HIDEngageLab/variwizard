/**
 * \file wizard_config.cpp
 * \author Roman Koch (koch.roman@gmail.com)
 *
 * Copyright (c) 2024, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "rapidjson/document.h"

#include "wizard_config.hpp"

namespace wizard
{
    namespace config
    {

        static char const *const device_tag = "device";
        static char const *const serial_tag = "serial";

        std::string device_path{"/dev/hidraw"};
        uint32_t serial_number{0};

        extern void load(void)
        {
            std::string config_content;

            std::ifstream config_file;
            config_file.open("config.json", std::ios::in | std::ios::binary);
            if (config_file.is_open())
            {
                std::stringstream buffer;
                buffer << config_file.rdbuf();
                config_content = buffer.str();
                config_file.close();

                rapidjson::Document document;
                document.Parse(config_content.c_str());

                if (document.HasMember(device_tag))
                {
                    if (document[device_tag].IsString())
                    {
                        device_path = std::string(document[device_tag].GetString());
                    }
                }
                if (document.HasMember(serial_tag))
                {
                    if (document[serial_tag].IsString())
                    {
                        serial_number = atoi(document[serial_tag].GetString());
                    }
                }
            }
            else
            {
                std::ofstream new_config;
                new_config.open("config.json", std::ios::out | std::ios::binary);
                if (new_config.is_open())
                {
                    new_config << "{\n";
                    new_config << "\"" << device_tag << "\":\"" << device_path << "\",\"" << serial_tag << "\":\"" << serial_number << "\"";
                    new_config << "}\n";
                    new_config.close();
                }
            }
        }

        extern std::string get_device()
        {
            return device_path;
        }

        extern uint32_t get_serial()
        {
            return serial_number;
        }
    }
}