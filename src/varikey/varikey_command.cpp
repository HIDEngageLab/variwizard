/**
 * \file varikey_command.cpp
 * \author Koch, Roman (koch.roman@googlemail.com)
 *
 * Copyright (c) 2024, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include "varikey_command.hpp"

namespace varikey
{
    namespace parameter
    {
        extern const int id2int(const IDENTIFIER _identifier)
        {
            switch (_identifier)
            {
            case varikey::parameter::IDENTIFIER::BACKLIGHT:
                return 0xA1;
            case varikey::parameter::IDENTIFIER::DISPLAY:
                return 0xA3;
            case varikey::parameter::IDENTIFIER::FEATURES:
                return 0x51;
            case varikey::parameter::IDENTIFIER::KEYPAD:
                return 0xA2;
            case varikey::parameter::IDENTIFIER::MAINTAINER:
                return 0x23;
            case varikey::parameter::IDENTIFIER::MAPPING:
                return 0xB0;
            case varikey::parameter::IDENTIFIER::POSITION:
                return 0x24;
            case varikey::parameter::IDENTIFIER::SERIAL_NUMBER:
                return 0x11;
            case varikey::parameter::IDENTIFIER::USER:
                return 0x70;
            default:
                break;
            }
            return 0xff;
        }

        extern const IDENTIFIER int2id(const int _identifier)
        {
            switch (_identifier)
            {
            case 0xA1:
                return varikey::parameter::IDENTIFIER::BACKLIGHT;
            case 0xA3:
                return varikey::parameter::IDENTIFIER::DISPLAY;
            case 0x51:
                return varikey::parameter::IDENTIFIER::FEATURES;
            case 0xA2:
                return varikey::parameter::IDENTIFIER::KEYPAD;
            case 0x23:
                return varikey::parameter::IDENTIFIER::MAINTAINER;
            case 0xB0:
                return varikey::parameter::IDENTIFIER::MAPPING;
            case 0x24:
                return varikey::parameter::IDENTIFIER::POSITION;
            case 0x11:
                return varikey::parameter::IDENTIFIER::SERIAL_NUMBER;
            case 0x70:
                return varikey::parameter::IDENTIFIER::USER;
            default:
                break;
            }
            return varikey::parameter::IDENTIFIER::UNDEFINED;
        }

        extern const int size(const IDENTIFIER _identifier)
        {
            switch (_identifier)
            {
            case varikey::parameter::IDENTIFIER::BACKLIGHT:
                return sizeof(varikey::parameter::backlight_t);
            case varikey::parameter::IDENTIFIER::DISPLAY:
                return sizeof(varikey::parameter::display_t);
            case varikey::parameter::IDENTIFIER::FEATURES:
                return sizeof(varikey::parameter::features_t);
            case varikey::parameter::IDENTIFIER::KEYPAD:
                return sizeof(varikey::parameter::keypad_t);
            case varikey::parameter::IDENTIFIER::MAINTAINER:
                return sizeof(varikey::parameter::maintainer_t);
            case varikey::parameter::IDENTIFIER::MAPPING:
                return sizeof(varikey::parameter::mapping_t);
            case varikey::parameter::IDENTIFIER::POSITION:
                return sizeof(varikey::parameter::position_t);
            case varikey::parameter::IDENTIFIER::SERIAL_NUMBER:
                return sizeof(varikey::parameter::serial_number_t);
            case varikey::parameter::IDENTIFIER::USER:
                return sizeof(varikey::parameter::user_t);
            default:
                break;
            }
            return 0;
        }
    }
}
