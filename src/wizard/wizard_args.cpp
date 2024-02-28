/**
 * \file wizard_args.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <argp.h>
#include <cstdio>
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

        {"gadget", 'G', "STATE", 0, "gadget control: 0=MOUNT, 1=UNMOUNT, 2=SUSPEND and 3=RESUME", 60},

        {"keycode", 'k', "CODE", 0, "simulate key matrix code (0-23)", 70},
        {"enable-interface", 'I', "INTERFACE", 0, "enable events over interface 0=HCI, 1=HID", 70},
        {"disable-interface", 'D', "INTERFACE", 0, "disable events over interface 0=HCI, 1=HID", 70},
        {"set-matting", 's', "MAP", 0, "set keypad mapping 0=NUMBER, 1=FUNCTIONAL, 2=NAVIGATION, 3=TELEFON, 4=MULTIMEDIA,  5=CUSTOM", 70},
        {"get-mapping", 'M', 0, 0, "get current mapping", 70},
        {"clean-mapping", 'X', 0, 0, "clean mapping states", 70},

        {"set-input", '2', "PIN", 0, "set pin 0-3 to input", 80},
        {"set-output", '3', "PIN", 0, "set pin 0-3 to output", 80},
        {"direction", '4', "PIN", 0, "get pin 0-3 direction", 80},
        {"set-high", 'H', "PIN", 0, "set output pin 0-3 high", 80},
        {"set-low", 'L', "PIN", 0, "set output pin 0-3 low", 80},
        {"get-level", 'Y', "PIN", 0, "get pin 0-3 level", 80},
        {"enable-pin", 'A', "PIN", 0, "enable alarm on pin 0-3", 80},
        {"disable-pin", 'a', "PIN", 0, "disable alarm on pin 0-3", 80},

        {"get-parameter", 'U', "PARAMETER", 0, "get parameter: 161=BACKLIGHT, 163=DISPLAY, 81=FEATURES, 162=KEYPAD, 35=MAINTAINER, 176=MAPPING, 36=POSITION, 17=SERIAL_NUMBER, 112=USER", 90},
        {"set-parameter", 'u', "PARAMETER", 0, "set parameter", 90},

        {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct wizard::arguments *arguments = (struct wizard::arguments *)state->input;
    switch (key)
    {
    case 'l':
        arguments->list = true;
        arguments->devices_directory = arg;
        break;
    case 'p':
        arguments->pid = std::stoi(arg);
        break;
    case 'v':
        arguments->vid = std::stoi(arg);
        break;
    case 'r':
        arguments->reset = true;
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
    case 'f':
        arguments->font_size = std::stoi(arg);
        break;
    case 'm':
        arguments->text = arg;
        break;
    case 'i':
        arguments->icon = std::stoi(arg);
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
    case 'b':
        arguments->backlight = std::stoi(arg);
        break;

    case 't':
        arguments->temperature = true;
        break;

    case 'G':
        arguments->gadget_state = std::stoi(arg);
        break;

    case 'k':
        arguments->key_code = std::stoi(arg);
        break;

    case 'I':
    {
        uint8_t identifier = std::stoi(arg);
        if (identifier == 0)
        {
            arguments->enable_hci = true;
            arguments->enable_hci_value = true;
        }
        else
        {
            arguments->enable_hid = true;
            arguments->enable_hid_value = true;
        }
        break;
    }
    case 'D':
    {
        uint8_t identifier = std::stoi(arg);
        if (identifier == 0)
        {
            arguments->enable_hci = true;
            arguments->enable_hci_value = false;
        }
        else
        {
            arguments->enable_hid = true;
            arguments->enable_hid_value = false;
        }
        break;
    }
    case 's':
        arguments->mapping = std::stoi(arg);
        break;
    case 'M':
        arguments->mapping = 0x77;
        break;
    case 'X':
        arguments->clean_mapping = true;
        break;

    case '2':
        arguments->pin = std::stoi(arg) % 4;
        arguments->direction = wizard::DIRECTION::DIRECTION_IN;
        break;
    case '3':
        arguments->pin = std::stoi(arg) % 4;
        arguments->direction = wizard::DIRECTION::DIRECTION_OUT;
        break;
    case '4':
        arguments->pin = std::stoi(arg) % 4;
        arguments->get_direction = true;
        break;
    case 'H':
        arguments->pin = std::stoi(arg) % 4;
        arguments->level = wizard::LEVEL::LEVEL_HIGH;
        break;
    case 'L':
        arguments->pin = std::stoi(arg) % 4;
        arguments->level = wizard::LEVEL::LEVEL_LOW;
        break;
    case 'Y':
        arguments->pin = std::stoi(arg) % 4;
        arguments->get_level = true;
        break;
    case 'A':
        arguments->pin = std::stoi(arg) % 4;
        arguments->alarm = wizard::ALARM::ALARM_ENABLE;
        break;
    case 'a':
        arguments->pin = std::stoi(arg) % 4;
        arguments->alarm = wizard::ALARM::ALARM_DISABLE;
        break;

    case 'U':
        arguments->parameter = std::stoi(arg);
        arguments->parameter_function = wizard::FUNCTION::FUNCTION_GET;
        break;
    case 'u':
        arguments->parameter = std::stoi(arg);
        arguments->parameter_function = wizard::FUNCTION::FUNCTION_SET;
        break;

    case ARGP_KEY_NO_ARGS:
        if (state->argc == 1)
            argp_usage(state);
        break;
    case ARGP_KEY_ARG:
        arguments->strings[state->arg_num] = arg;
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
    arguments.list = false;
    arguments.reset = false;
    arguments.verbose = false;

    arguments.clean = false;
    arguments.column = 0xff;
    arguments.line = 0xff;
    arguments.font_size = 0xff;
    arguments.text = nullptr;
    arguments.icon = 0xff;

    arguments.backlight = 0xff;
    arguments.r_left_value = 0xff;
    arguments.g_left_value = 0xff;
    arguments.b_left_value = 0xff;
    arguments.r_right_value = 0xff;
    arguments.g_right_value = 0xff;
    arguments.b_right_value = 0xff;

    arguments.temperature = false;

    arguments.gadget_state = 0xff;

    arguments.key_code = 0xff;
    arguments.enable_hci = false;
    arguments.enable_hci_value = false;
    arguments.enable_hid = false;
    arguments.enable_hid_value = false;
    arguments.mapping = 0xff;
    arguments.clean_mapping = false;

    arguments.pin = 0xff;
    arguments.direction = wizard::DIRECTION_UNDEFINED;
    arguments.get_direction = false;
    arguments.level = wizard::LEVEL_UNDEFINED;
    arguments.get_level = false;
    arguments.alarm = wizard::ALARM_UNDEFINED;

    arguments.parameter = 0xff;
    arguments.parameter_function = wizard::FUNCTION::FUNCTION_UNDEFINED;
}

extern void wizard_arguments_parse(wizard::arguments &arguments, int argc, char *argv[])
{
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
}
