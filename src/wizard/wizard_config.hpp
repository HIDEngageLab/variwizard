/**
 * \file wizard_config.hpp
 * \author Roman Koch (koch.roman@gmail.com)
 *
 * Copyright (c) 2024, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __WIZARD_CONFIG_HPP__
#define __WIZARD_CONFIG_HPP__

#include <cstdint>
#include <string>

namespace wizard
{
    namespace config
    {
        extern void load(void);

        extern std::string get_device();
        extern uint32_t get_serial();
    }
}

#endif // __WIZARD_CONFIG_HPP__
