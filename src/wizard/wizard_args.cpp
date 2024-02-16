/**
 * \file wizard_args.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <argp.h>
#include <cstdlib>
#include <iostream>

#include "wizard_args.hpp"
#include "wizard_revision.h"

/**
 * \brief USB device identifiers
 *
 * 0xcafe=51966, 0x4004=16388
 * attention: devices are filtered by product:vendor values
 * @{
 */
#define DEFAULT_VENDOR_IDENTIFIER 0xbeef
#define DEFAULT_PRODUCT_IDENTIFIER 0x0024
/** }@ */

wizard::arguments arguments = {};

const char *argp_program_version = REVISION();
const char *argp_program_bug_address = ADDRESS();

static struct argp_option options[] =
    {
        {"list", 'l', "PATH", 0, "devices list", 10},
        {"pid", 'p', "PID", 0, "product identifier", 10},
        {"vid", 'v', "VID", 0, "vendor identifier", 10},
        {"reset", 'r', 0, 0, "reset wizard device", 10},
        {"verbose", 'V', 0, 0, "more output", 10},
        {"clean", 'c', 0, 0, "clean up display", 20},
        {"column", 'x', "COLUMN", 0, "set the column for the next output (0-127)", 20},
        {"line", 'y', "LINE", 0, "set the line for the next output (0-3)", 20},
        {"font", 'f', "FONT", 0, "set font size (check the docs)", 20},
        {"message", 'm', "TEXT", 0, "show message string on gadget", 20},
        {"icon", 'i', "ICON", 0, "draw predefined icon (check the docs)", 30},
        {"backcolor", 'B', "RGB", OPTION_ARG_OPTIONAL, "set the backlight color with hex RRGGBB (use modes 3 and 6)", 40},
        {"left", '6', "COLOR", 0, "set the left backlight color with hex RRGGBB (use with -B)", 40},
        {"right", '7', "COLOR", 0, "set the right backlight color with hex RRGGBB (use with -B)", 40},
        {"backlight", 'b', "MODE", 0, "set the backlight mode (1-8,170, 171)", 40},
        {"temperature", 't', 0, 0, "show gadget processor temperature", 50},
        {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct wizard::arguments *arguments = (struct wizard::arguments *)state->input;
    switch (key)
    {
    case 'b':
        arguments->backlight = std::stoi(arg);
        break;
    case 'B':
    {
        arguments->backlight = 3;
        if (arg)
        {
            std::string color(arg);
            if (color.length() == 6)
            {
                int r, g, b;
                sscanf(color.substr(0, 2).c_str(), "%x", &r);
                sscanf(color.substr(2, 2).c_str(), "%x", &g);
                sscanf(color.substr(4, 2).c_str(), "%x", &b);

                arguments->r_left_value = static_cast<uint8_t>(r & 0xff);
                arguments->g_left_value = static_cast<uint8_t>(g & 0xff);
                arguments->b_left_value = static_cast<uint8_t>(b & 0xff);
                arguments->r_right_value = arguments->r_left_value;
                arguments->g_right_value = arguments->g_left_value;
                arguments->b_right_value = arguments->b_left_value;
            }
            else
            {
                perror("color like RRGGBB expected");
            }
        }
        break;
    }
    case '6':
    {
        std::string color(arg);
        if (color.length() == 6)
        {
            int r, g, b;
            sscanf(color.substr(0, 2).c_str(), "%x", &r);
            sscanf(color.substr(2, 2).c_str(), "%x", &g);
            sscanf(color.substr(4, 2).c_str(), "%x", &b);

            arguments->r_left_value = static_cast<uint8_t>(r & 0xff);
            arguments->g_left_value = static_cast<uint8_t>(g & 0xff);
            arguments->b_left_value = static_cast<uint8_t>(b & 0xff);
        }
        else
        {
            perror("color like RRGGBB expected");
        }
        break;
    }
    case '7':
    {
        std::string color(arg);
        if (color.length() == 6)
        {
            int r, g, b;
            sscanf(color.substr(0, 2).c_str(), "%x", &r);
            sscanf(color.substr(2, 2).c_str(), "%x", &g);
            sscanf(color.substr(4, 2).c_str(), "%x", &b);

            arguments->r_right_value = static_cast<uint8_t>(r & 0xff);
            arguments->g_right_value = static_cast<uint8_t>(g & 0xff);
            arguments->b_right_value = static_cast<uint8_t>(b & 0xff);
        }
        else
        {
            perror("color like RRGGBB expected");
        }
        break;
    }
    case 'p':
        arguments->pid = std::stoi(arg);
        break;
    case 'v':
        arguments->vid = std::stoi(arg);
        break;
    case 'f':
        arguments->font_size = std::stoi(arg);
        break;
    case 'i':
        arguments->icon = std::stoi(arg);
        break;
    case 'l':
        arguments->list = true;
        arguments->devices_directory = arg;
        break;
    case 'm':
        arguments->text = arg;
        break;
    case 'r':
        arguments->reset = true;
        break;
    case 't':
        arguments->temperature = true;
        break;
    case 'V':
        arguments->verbose = true;
        break;
    case 'c':
        arguments->clean = true;
        break;
    case 'x':
        arguments->column = std::stoi(arg);
        break;
    case 'y':
        arguments->line = std::stoi(arg);
        break;
    case ARGP_KEY_NO_ARGS:
        if (state->argc == 1)
            argp_usage(state);
        break;
    case ARGP_KEY_ARG:
        break;
    case ARGP_KEY_END:
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static char doc[] = "gadget controller";
static struct argp argp = {options, parse_opt, 0, doc, NULL, NULL, NULL};

/**
 * @brief set argument defaults
 *
 * @param arguments program arguments
 */
extern void wizard_arguments_init(wizard::arguments &arguments)
{
    arguments.devices_directory = nullptr;
    arguments.pid = DEFAULT_PRODUCT_IDENTIFIER;
    arguments.vid = DEFAULT_VENDOR_IDENTIFIER;
    arguments.verbose = false;
    arguments.list = false;
    arguments.reset = false;
    arguments.line = 0xff;
    arguments.column = 0xff;
    arguments.icon = 0xff;
    arguments.font_size = 0xff;
    arguments.text = nullptr;
    arguments.temperature = false;
    arguments.backlight = 0xff;
    arguments.r_left_value = 0xff;
    arguments.g_left_value = 0xff;
    arguments.b_left_value = 0xff;
    arguments.r_right_value = 0xff;
    arguments.g_right_value = 0xff;
    arguments.b_right_value = 0xff;
}

extern void wizard_arguments_parse(wizard::arguments &arguments, int argc, char *argv[])
{
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
}
