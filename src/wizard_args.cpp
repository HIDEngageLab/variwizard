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

wizard::arguments arguments = {};

const char *argp_program_version = REVISION();
const char *argp_program_bug_address = ADDRESS();

static struct argp_option options[] =
    {
        {"backlight", 'b', "MODE", 0, "set the backlight mode (check the docs)", 40},
        {"device", 'd', "DEVICE", 0, "device path", 10},
        {"font", 'f', "FONT", 0, "set font size (check the docs)", 20},
        {"icon", 'i', "ICON", 0, "draw predefined icon (check the docs)", 30},
        {"list", 'l', "PATH", 0, "devices list", 10},
        {"message", 'm', "TEXT", 0, "show message string on gadget", 20},
        {"reset", 'r', 0, 0, "reset wizard device", 10},
        {"temperature", 't', 0, 0, "show gadget processor temperature", 50},
        {"unique", 'u', "UNIQUE", 0, "get unique gadget identifier", 10},
        {"verbose", 'v', 0, 0, "more output", 10},
        {"column", 'x', "COLUMN", 0, "set the column for the next output (0-127)", 20},
        {"line", 'y', "LINE", 0, "set the line for the next output (0-3)", 20},
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
    case 'd':
        arguments->device = arg;
        break;
    case 'f':
        arguments->font_size = std::stoi(arg);
        break;
    case 'i':
        arguments->icon = std::stoi(arg);
        break;
    case 'l':
        arguments->list = true;
        arguments->device = arg;
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
    case 'u':
        arguments->unique = std::stol(arg);
        break;
    case 'v':
        arguments->verbose = true;
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
static struct argp argp = {options, parse_opt, 0, doc, 0, 0, 0};

/**
 * @brief set argument defaults
 *
 * @param arguments program arguments
 */
extern void wizard_arguments_init(wizard::arguments &arguments)
{
    arguments.device = nullptr;
    arguments.verbose = false;
    arguments.unique = 0;
    arguments.list = false;
    arguments.reset = false;
    arguments.line = 0xff;
    arguments.column = 0xff;
    arguments.icon = 0xff;
    arguments.font_size = 0xff;
    arguments.text = nullptr;
    arguments.temperature = false;
    arguments.backlight = 0xff;
}

extern void wizard_arguments_parse(wizard::arguments &arguments, int argc, char *argv[])
{
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
}
