/**
 * \file wizard_args.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __WIZARD_ARGS_HPP__
#define __WIZARD_ARGS_HPP__

#include <cstdint>

namespace wizard
{
    struct arguments
    {
        const char *device; /* wizard device */
        uint32_t unique;    /* unique identifier */
        bool verbose;       /* verbose flag */
        bool list;          /* devices list */
        bool reset;         /* reset flag */
        uint8_t line;       /* line position  */
        uint8_t column;     /* column position */
        uint8_t icon;       /* predefined icon */
        uint8_t font_size;  /* text line */
        char *text;         /* text line */
        bool temperature;   /* get temperature */
        uint8_t backlight;  /* set backlight mode */
        uint8_t r_value;    /* set backlight red channel */
        uint8_t g_value;    /* set backlight green channel */
        uint8_t b_value;    /* set backlight blue channel */
    };
}

extern void wizard_arguments_init(wizard::arguments &);
extern void wizard_arguments_parse(wizard::arguments &, int argc, char *argv[]);

#endif // __WIZARD_ARGS_HPP__