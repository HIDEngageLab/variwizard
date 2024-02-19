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
        uint16_t pid; /* usb pid */
        uint16_t vid; /* usb vid */

        bool verbose; /* verbose flag */

        bool list;               /* devices list */
        char *devices_directory; /* text line */

        bool reset; /* reset flag */

        bool clean;        /* clean up display */
        uint8_t line;      /* line position  */
        uint8_t column;    /* column position */
        uint8_t icon;      /* predefined icon */
        uint8_t font_size; /* text line */
        char *text;        /* text line */

        bool temperature; /* get temperature */

        uint8_t backlight;     /* set backlight mode */
        uint8_t r_left_value;  /* set left backlight red channel */
        uint8_t g_left_value;  /* set left backlight green channel */
        uint8_t b_left_value;  /* set left backlight blue channel */
        uint8_t r_right_value; /* set right backlight red channel */
        uint8_t g_right_value; /* set right backlight green channel */
        uint8_t b_right_value; /* set right backlight blue channel */

        uint8_t gadget_state; /* gadget state */

        uint8_t key_code;      /* gadget state */
        bool enable_hci;       /* enable hci */
        bool enable_hci_value; /* enable hci */
        bool enable_hid;       /* enable hci */
        bool enable_hid_value; /* enable hid */
        uint8_t mapping;       /* key matrix mapping */
        bool clean_mapping;    /* clean internal mapping states */
    };
}

extern void wizard_arguments_init(wizard::arguments &);
extern void wizard_arguments_parse(wizard::arguments &, int argc, char *argv[]);

#endif // __WIZARD_ARGS_HPP__