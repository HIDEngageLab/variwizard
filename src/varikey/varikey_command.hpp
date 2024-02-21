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

#include "chunk.h"
#include "macros.hpp"
#include "varikey_device.hpp"

namespace varikey
{

    enum class COMMAND : uint8_t
    {
        KEYBOARD = 1,
        MOUSE = 2,
        CONSUMER = 3,
        CUSTOM = 0xE0,
    };

    enum class IDENTIFIER : uint8_t
    {
        BACKLIGHT = 0x10,
        DISPLAY = 0x30,
        GADGET = 0x40,
        GPIO = 0x50,
        IDENTITY = 0xA0,
        KEYCODE = 0x65,
        KEYPAD = 0x70,
        PARAMETER = 0xB0,
        RESET = 0xEE,
        TEMPERATURE = 0xC0,
        UNDEFINED = 0xFF,
    };

    namespace function
    {
        /** \brief Anonymous function data type */
        enum VALUE
        {
            CLEAN = 0x08,
            DISABLE = 0x03,
            ENABLE = 0x02,
            GET = 0x00,
            OFF = 0x07,
            ON = 0x06,
            SET = 0x01,
            START = 0x04,
            STOP = 0x05,

            CUSTOM = 0x80,

            UNDEFINED = 0xFF,
        };
    }

    constexpr int operator+(IDENTIFIER _report, int _value)
    {
        return static_cast<int>(_report) + _value;
    }

    namespace backlight
    {

        typedef union
        {
            uint8_t value[3];
            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
            } rgb;
        } color_t;

        enum class PROGRAM : uint8_t
        {
            ALERT = 0x00,
            CONST = 0x01,
            MEDIUM = 0x02,
            MORPH = 0x03,
            MOUNT = 0x04,
            OFF = 0x05,
            SET = 0x06,
            SLOW = 0x07,
            SUSPEND = 0x08,
            TURBO = 0x09,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            PROGRAM program;
            color_t color_left;
            color_t color_right;
        };
    }

    namespace display
    {
        enum class FUNCTION : uint8_t
        {
            CLEAN = varikey::IDENTIFIER::DISPLAY + 5,    /* clean up display */
            FONT = varikey::IDENTIFIER::DISPLAY + 1,     /* font art/size */
            ICON = varikey::IDENTIFIER::DISPLAY + 2,     /* icon identifier */
            POSITION = varikey::IDENTIFIER::DISPLAY + 3, /* cursor position */
            TEXT = varikey::IDENTIFIER::DISPLAY + 4,     /* text message */

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        enum class FONT : uint8_t
        {
            SMALL = 0x00, /* default */
            NORMAL = 0x01,
            BIG = 0x02,
            HUGE = 0x03,
            SYMBOL = 0x04,
            UNDEFINED = 0xff,
        };

        enum class ICON : uint16_t
        {
            FRAME = 0x0000,
            VARIKEY_LOGO = 0x0001,
            GOSSENMETRAWATT_LOGO = 0x0002,
            HEART = 0x0003,
            ADD_CIRCLE_8 = 0x0004,
            ADD_CIRCLE_16 = 0x0005,
            ADD_CIRCLE_24 = 0x0006,
            ADD_CIRCLE_32 = 0x0007,
            ADD_SQUARE_8 = 0x0008,
            ADD_SQUARE_16 = 0x0009,
            ADD_SQUARE_24 = 0x0010,
            ADD_SQUARE_32 = 0x0011,
            UNDEFINED = 0xffff,
        };

        struct position_t
        {
            uint8_t line;
            uint8_t column;
        };

        static const size_t MAX_TEXT_SIZE = 20;
        struct __attribute__((packed)) content_t
        {

            FUNCTION identifier;
            union
            {
                FONT font;
                position_t position;
                char text[MAX_TEXT_SIZE + 1];
                ICON icon;
            };
        };
    }

    namespace gadget
    {
        enum class MODE : uint8_t
        {
            ACTIVE = 0x01,
            IDLE = 0x00,
            PENDING = 0x03,
            SUSPEND = 0x02,
            UNDEFINED = 0xff,
        };

        enum class COMMAND : uint8_t
        {
            MOUNT = varikey::IDENTIFIER::GADGET + 1,
            RESUME = varikey::IDENTIFIER::GADGET + 4,
            SUSPEND = varikey::IDENTIFIER::GADGET + 3,
            UNMOUNT = varikey::IDENTIFIER::GADGET + 2,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            COMMAND command;
            MODE mode;
        };
    }

    namespace gpio
    {
        enum class FUNCTION : uint8_t
        {
            DISABLE = function::DISABLE,
            ENABLE = function::ENABLE,

            DIRECTION_GET = function::CUSTOM,
            DIRECTION_SET = function::CUSTOM + 1,
            LEVEL_GET = function::CUSTOM + 4,
            LEVEL_SET = function::CUSTOM + 5,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        enum class IDENTIFIER : uint8_t
        {
            PIN1 = varikey::IDENTIFIER::GPIO + 0x0a,
            PIN2 = varikey::IDENTIFIER::GPIO + 0x0b,
            PIN3 = varikey::IDENTIFIER::GPIO + 0x0c,
            PIN4 = varikey::IDENTIFIER::GPIO + 0x0d,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        /** \brief GPIO message direction code */
        enum class DIRECTION : uint8_t
        {
            INPUT = 0x00,
            OUTPUT = 0x01,
            UNDEFINED = 0xFF,
        };

        enum class LEVEL : uint8_t
        {
            LOW = 0x00,
            HIGH = 0x01,
            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            FUNCTION function;
            IDENTIFIER identifier;
            union
            {
                DIRECTION direction;
                LEVEL state;
            };
        };
    }

    namespace identity
    {
        enum class IDENTIFIER : uint8_t
        {
            FIRMWARE = varikey::IDENTIFIER::IDENTITY + 1,
            HARDWARE = varikey::IDENTIFIER::IDENTITY + 2,
            PLATFORM = varikey::IDENTIFIER::IDENTITY + 4,
            PRODUCT = varikey::IDENTIFIER::IDENTITY + 3,
            SERIAL = varikey::IDENTIFIER::IDENTITY + 5,
            UNIQUE = varikey::IDENTIFIER::IDENTITY + 6,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            IDENTIFIER identifier;
            union
            {
                uint8_t buffer[PLATFORM_SIZE + PRODUCT_SIZE];
                struct
                {
                    uint16_t identifier;
                    uint16_t revision;
                    uint16_t patch;
                    uint16_t build;
                    uint16_t vendor;
                } firmware;
                struct
                {
                    uint16_t maintainer;
                    uint16_t identifier;
                    uint8_t number;
                    uint8_t variant;
                } hardware;
                char product[PRODUCT_SIZE];
                char platform[PLATFORM_SIZE];
                uint8_t serial[SERIAL_NUMBER_SIZE];
                uint32_t unique;
            };
        };
    }

    namespace keycode
    {
        enum class KEY_ID : uint8_t
        {
            KEY_01 = 0x00,
            KEY_02 = 0x01,
            KEY_03 = 0x02,
            KEY_04 = 0x03,
            KEY_05 = 0x04,
            KEY_06 = 0x05,
            KEY_07 = 0x06,
            KEY_08 = 0x07,
            KEY_09 = 0x08,
            KEY_10 = 0x09,

            KEY_70 = 0x0a, /* wheel 1 up */
            KEY_71 = 0x0b, /* wheel 1 down */
            KEY_72 = 0x0c, /* wheel 1 switch */
            KEY_73 = 0x0d, /* wheel 2 up */
            KEY_74 = 0x0e, /* wheel 2 down */
            KEY_75 = 0x0f, /* wheel 2 switch */

            KEY_80 = 0x10, /* joystick 1 down */
            KEY_81 = 0x11, /* joystick 1 left */
            KEY_82 = 0x12, /* joystick 1 right */
            KEY_83 = 0x13, /* joystick 1 up */
            KEY_84 = 0x14, /* joystick 2 down */
            KEY_85 = 0x15, /* joystick 2 left */
            KEY_86 = 0x16, /* joystick 2 right */
            KEY_87 = 0x17, /* joystick 2 up */

            UNDEFINED = 0xff,
        };

        enum class STATE : uint8_t
        {
            PRESS = 0x00,
            RELEASE = 0x01,
            UNDEFINED = 0xff,
        };

        enum class TABLE : uint8_t
        {
            CUSTOM = 0x05,
            FUNCTIONAL = 0x01,
            MULTIMEDIA = 0x04,
            NAVIGATION = 0x02,
            NUMBER = 0x00,
            TELEFON = 0x03,

            UNDEFINED = 0xff,
        };

        enum class CONTROL : uint8_t
        {
            BUTTON = 0x01,
            JOYSTICK_1 = 0x04,
            JOYSTICK_2 = 0x05,
            KEYPAD = 0x06,
            WHEEL_1 = 0x02,
            WHEEL_2 = 0x03,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            static const size_t CFM_SIZE = 3;

            CONTROL control;
            KEY_ID key_id;
            STATE state;
            TABLE table;
        };
    }

    namespace keypad
    {
        using TABLE = varikey::keycode::TABLE;

        const uint8_t KEYPAD_ID = int(varikey::IDENTIFIER::KEYPAD);

        enum class IDENTIFIER : uint8_t
        {
            HCI = KEYPAD_ID + 2,
            HID = KEYPAD_ID + 3,
            KEYCODE = KEYPAD_ID + 4,
            MAPPING = KEYPAD_ID + 1,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        enum class FUNCTION : uint8_t
        {
            DISABLE = function::DISABLE,
            ENABLE = function::ENABLE,
            GET = function::GET,
            SET = function::SET,
            CLEAN = function::CLEAN,

            CLICK = function::CUSTOM,
            PUSH = function::CUSTOM + 1,

            UNDEFINED = (int)varikey::function::UNDEFINED,
        };

        enum MODIFIER
        {
            KEY_MOD_LCTRL = 0x01,
            KEY_MOD_LSHIFT = 0x02,
            KEY_MOD_LALT = 0x04,
            KEY_MOD_LMETA = 0x08,
            KEY_MOD_RCTRL = 0x10,
            KEY_MOD_RSHIFT = 0x20,
            KEY_MOD_RALT = 0x40,
            KEY_MOD_RMETA = 0x80,
        };

        struct __attribute__((packed)) content_t
        {
            IDENTIFIER identifier;
            FUNCTION function;
            union
            {
                TABLE table;
                uint8_t code;
            };
        };
    }

    namespace parameter
    {
        static const size_t SIZE_BACKLIGHT = sizeof(varikey::backlight::PROGRAM) +
                                             sizeof(varikey::backlight::color_t) +
                                             sizeof(varikey::backlight::color_t) +
                                             sizeof(uint16_t);
        static const size_t SIZE_DISPLAY = sizeof(uint8_t) * 1;
        static const size_t SIZE_FEATURES = sizeof(uint16_t);
        static const size_t SIZE_KEYPAD = sizeof(uint16_t) * 2;
        static const size_t SIZE_MAINTAINER = sizeof(uint16_t) * 1;
        static const size_t SIZE_MAPPING = 24;
        static const size_t SIZE_POSITION = sizeof(float) * 2;
        static const size_t SIZE_SERIAL_NUMBER = 12;
        static const size_t SIZE_USER = 2;

        union backlight_t
        {
            uint8_t byte[SIZE_BACKLIGHT];
            struct __attribute__((packed))
            {
                varikey::backlight::PROGRAM mode;
                varikey::backlight::color_t left;
                varikey::backlight::color_t right;
                uint16_t timeout;
            } backlight;
        };

        union display_t
        {
            uint8_t byte[SIZE_DISPLAY];
            struct __attribute__((packed))
            {
                ability_t rotate : 1;
                ability_t inverse : 1;
                ability_t slides : 1;
                uint8_t reserved : 5;
            } display;
        };

        union features_t
        {
            uint8_t byte[SIZE_FEATURES];
            struct __attribute__((packed))
            {
                ability_t autostart : 1;
                ability_t display : 1;
                ability_t keypad : 1;
                ability_t wakeup : 1;
                uint16_t reserved : 12;
            } features;
        };

        union keypad_t
        {
            uint8_t byte[SIZE_KEYPAD];
            struct __attribute__((packed))
            {
                uint16_t click_ms;
                uint16_t push_ms;
            } keypad;
        };

        union maintainer_t
        {
            uint16_t word;
            struct __attribute__((packed))
            {
                uint16_t protocol : 2;
                uint16_t hardware : 4;
                uint16_t identifier : 12;
            } maintainer;
        };

        struct mapping_t
        {
            uint8_t value[SIZE_MAPPING];
        };

        union position_t
        {
            uint8_t byte[SIZE_POSITION];
            struct __attribute__((packed))
            {
                float latitude;
                float longitude;
            } coordinates;
        };

        struct serial_number_t
        {
            uint8_t value[SIZE_SERIAL_NUMBER];
        };

        struct user_t
        {
            uint8_t value[SIZE_USER];
        };

        enum class IDENTIFIER : uint8_t
        {
            BACKLIGHT = 0xA1,
            DISPLAY = 0xA3,
            FEATURES = 0x51,
            KEYPAD = 0xA2,
            MAINTAINER = 0x23,
            MAPPING = 0xB0,
            POSITION = 0x24,
            SERIAL_NUMBER = 0x11,
            USER = 0x70,

            UNDEFINED = 0xff,
        };

        extern const int id2int(const IDENTIFIER);
        extern const IDENTIFIER int2id(const int);
        extern const int size(const IDENTIFIER);

        enum class FUNCTION : uint8_t
        {
            SET = 0x01,
            GET = 0x00,

            UNDEFINED = 0xff,
        };

        struct __attribute__((packed)) content_t
        {
            IDENTIFIER identifier;
            FUNCTION function;
            union
            {
                uint buffer[100];
                backlight_t backlight;
                display_t display;
                features_t features;
                keypad_t keypad;
                maintainer_t maintainer;
                mapping_t mapping;
                position_t position;
                serial_number_t serial_number;
                user_t user;
            };
        };
    }

    namespace reset
    {
        enum class FUNCTION : uint8_t
        {
            SHUTDOWN = function::CUSTOM,
            FORMAT = function::CUSTOM + 1,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            FUNCTION function;
        };
    }

    namespace temperature
    {
        enum class FUNCTION : uint8_t
        {
            GET = 0x01,
            ALARM = function::CUSTOM,

            UNDEFINED = (int)varikey::IDENTIFIER::UNDEFINED,
        };

        struct __attribute__((packed)) content_t
        {
            FUNCTION function;
            float value;
        };
    }

    enum class SET_REPORT : uint8_t
    {
        BACKLIGHT = (int)IDENTIFIER::BACKLIGHT,
        DISPLAY = (int)IDENTIFIER::DISPLAY,
        GADGET = (int)IDENTIFIER::GADGET,
        GPIO = (int)IDENTIFIER::GPIO,
        KEYPAD = (int)IDENTIFIER::KEYPAD,
        PARAMETER = (int)IDENTIFIER::PARAMETER,
        RESET = (int)IDENTIFIER::RESET,

        UNDEFINED = 0xff,
    };

    struct __attribute__((__packed__)) set_report_t
    {
        COMMAND command;
        SET_REPORT report;
        union __attribute__((__packed__))
        {
            backlight::content_t backlight;
            display::content_t display;
            gadget::content_t gadget;
            gpio::content_t gpio;
            identity::content_t identity;
            keycode::content_t keycode;
            keypad::content_t keypad;
            parameter::content_t parameter;
            reset::content_t reset;
            temperature::content_t temperature;
        };
    };

    enum class GET_REPORT : uint8_t
    {
        FIRMWARE = (int)varikey::identity::IDENTIFIER::FIRMWARE,
        GPIO = (int)varikey::IDENTIFIER::GPIO,
        HARDWARE = (int)varikey::identity::IDENTIFIER::HARDWARE,
        MAPPING = (int)varikey::keypad::IDENTIFIER::MAPPING,
        PARAMETER = (int)varikey::IDENTIFIER::PARAMETER,
        SERIAL = (int)varikey::identity::IDENTIFIER::SERIAL,
        TEMPERATURE = (int)varikey::IDENTIFIER::TEMPERATURE,
        UNIQUE = (int)varikey::identity::IDENTIFIER::UNIQUE,

        UNDEFINED = 0xff,
    };

    enum class RESULT : uint8_t
    {
        CUSTOM = common::result::CUSTOM,
        ERROR = common::result::ERROR,
        FAILURE = common::result::FAILURE,
        SUCCESS = common::result::SUCCESS,
        UNKNOWN = common::result::UNKNOWN,
        UNSUPPORTED = common::result::UNSUPPORTED,

        UNDEFINED = (int)common::result::UNDEFINED,
    };

    struct __attribute__((__packed__)) get_report_t
    {
        GET_REPORT report;
        RESULT result;
        union
        {
            gpio::content_t gpio;
            identity::content_t identity;
            keypad::content_t keypad;
            parameter::content_t parameter;
            temperature::content_t temperature;
        };
    };
}

#endif /* __VARIKEY_COMMAND_HPP__ */
