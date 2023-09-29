/**
 * \file varikey_command.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __VARIKEY_COMMAND_HPP__
#define __VARIKEY_COMMAND_HPP__

#include <cstdint>

namespace varikey
{
    enum class report_id : unsigned char
    {
        CUSTOM = 6,
        SERIAL = 7,
        GADGET = 8,
        UNIQUE = 9,
        HARDWARE = 10,
        VERSION = 11,
        TEMPERATURE = 12,
    };
    enum class command_id : unsigned char
    {
        RESET = 1,
        POSITION = 2,
        ICON = 3,
        FONT_SIZE = 4,
        TEXT = 5,
        BACKLIGHT = 6
    };

    struct __attribute__((__packed__)) command
    {
        uint8_t report;
        uint8_t command;
        union
        {
            struct
            {
                uint8_t line;
                uint8_t column;
            } position;
            uint8_t byte_value;
            uint8_t text[40];
        } payload;
    };

    struct __attribute__((__packed__)) feature
    {
        uint8_t report;
        union
        {
            uint8_t serial[12];
            uint8_t byte_value;
            uint32_t long_value;
        } payload;
    };
}

#endif /* __VARIKEY_COMMAND_HPP__ */
