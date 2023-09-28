/**
 * \file varikey_gadget.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __VARIKEY_GADGET_HPP__
#define __VARIKEY_GADGET_HPP__

namespace varikey
{
    namespace gadget
    {
        enum class type : unsigned char
        {
            ILLEGAL = 0,
            DEFAULT = 0xa0,   /* 10 buttons, rotary encoder */
            BACKLIGHT = 0xa1, /* 10 buttons, rotary encoder, backlight */
            DISPLAY = 0xa3,   /* 10 buttons, rotary encoder, backlight, 128x32 display */
            EXTENDED = 0xa9,  /* reserved */
        };
    }
};

#endif // __VARIKEY_GADGET_HPP__
