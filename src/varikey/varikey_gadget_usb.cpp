/**
 * \file varikey_gadget_usb.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/hiddev.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <unistd.h>

#include "macros.hpp"
#include "varikey_gadget_usb.hpp"
#include "wizard.hpp"

namespace varikey
{
    namespace gadget
    {

        static const int INVALID_HANDLE_VALUE = 0xffff;

        /**
         * \brief Construct a new wizard usb entity::wizard usb entity object
         */
        usb::usb() : device_handle(INVALID_HANDLE_VALUE),
                     device_valid(false) {}

        /**
         * \brief Destroy the wizard usb entity::wizard usb entity object
         */
        usb::~usb()
        {
            usb_close();
        }

        /**
         * \brief open usb device defined by path
         *
         * @param _device_path
         */
        void usb::usb_open(const char *_device_path)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
                usb_close();

            device_handle = open(_device_path, O_RDWR);
            if (device_handle < 0)
            {
                device_handle = INVALID_HANDLE_VALUE;
                device_valid = false;
                return;
            }

            struct hidraw_devinfo dinfo;
            if (ioctl(device_handle, HIDIOCGRAWINFO, (void *)&dinfo) < 0)
            {
                usb_close();
                return;
            }

            device.bustype = dinfo.bustype;
            device.product = dinfo.product;
            device.vendor = dinfo.vendor;

            if (ioctl(device_handle, HIDIOCGRAWNAME(NAME_SIZE), (void *)&device.name) < 0)
            {
                perror("error sending report");
            }
        }

        /**
         * \brief close device
         */
        void usb::usb_close()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
            }
        }

        /**
         * \brief initialize device
         */
        void usb::usb_init()
        {
            device_valid = true;
            device_valid &= usb_get_serial();
            device_valid &= usb_get_unique();
            device_valid &= usb_get_hardware();
            device_valid &= usb_get_firmware();
        }

        /**
         * \brief reset and close an open device
         */
        void usb::reset_device()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::RESET;
                cmd.reset.function = varikey::reset::FUNCTION::SHUTDOWN;

                if (send_report(cmd, 3) < 0)
                {
                    usb_close();
                }
            }
        }

        /**
         * \brief check for open state
         *
         * \return true
         * \return false
         */
        bool usb::is_open() const { return device_handle != INVALID_HANDLE_VALUE; }

        /**
         * \brief set backlight mode for varikey gadget
         *
         * check varikey documentation for available modes
         *
         * @param mode
         */
        void usb::set_backlight_mode(const uint8_t mode)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::BACKLIGHT;
                cmd.backlight.program = static_cast<const varikey::backlight::PROGRAM>(mode);

                if (cmd.backlight.program == varikey::backlight::PROGRAM::ALERT ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::CONST ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::MEDIUM ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::MOUNT ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::OFF ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::SLOW ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::SUSPEND ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::TURBO)
                {
                    if (send_report(cmd, 3) < 0)
                    {
                        perror("send_report failed");
                        usb_close();
                    }
                }
                else
                {
                    perror("unknown backlight program");
                }
            }
        }

        /**
         * \brief set backlight color for varikey gadget
         *
         * \param r red channel
         * \param g green channel
         * \param b blue channel
         */
        void usb::set_backlight_color(const uint8_t mode,
                                      const uint8_t lr, const uint8_t lg, const uint8_t lb,
                                      const uint8_t rr, const uint8_t rg, const uint8_t rb)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::BACKLIGHT;
                cmd.backlight.program = static_cast<const varikey::backlight::PROGRAM>(mode);

                if (cmd.backlight.program == varikey::backlight::PROGRAM::MORPH ||
                    cmd.backlight.program == varikey::backlight::PROGRAM::SET)
                {
                    cmd.backlight.color_left.rgb.r = lr;
                    cmd.backlight.color_left.rgb.g = lg;
                    cmd.backlight.color_left.rgb.b = lb;
                    cmd.backlight.color_right.rgb.r = rr;
                    cmd.backlight.color_right.rgb.g = rg;
                    cmd.backlight.color_right.rgb.b = rb;

                    if (send_report(cmd, 9) < 0)
                    {
                        perror("send_report failed");
                        usb_close();
                    }
                }
                else
                {
                    perror("unknown backlight program");
                }
            }
        }

        /**
         * \brief set messsage position on varikey display
         *
         * @param line
         * @param column
         */
        void usb::set_position(const int line, const int column)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::DISPLAY;
                cmd.display.identifier = varikey::display::FUNCTION::POSITION;
                cmd.display.position.line = line;
                cmd.display.position.column = column;

                if (send_report(cmd, 5) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief clean up info display
         */
        void usb::clean_display()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::DISPLAY;
                cmd.display.identifier = varikey::display::FUNCTION::CLEAN;

                if (send_report(cmd, 3) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief draw an icon defined by integer identifier
         *
         * check varikey documentation for available icons
         *
         * @param icon
         */
        void usb::draw_icon(const int icon)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::DISPLAY;
                cmd.display.identifier = varikey::display::FUNCTION::ICON;
                cmd.display.icon = static_cast<varikey::display::ICON>(icon);

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief set on of the available font sizes
         *
         * check varikey documentation for available icons
         *
         * @param font_size
         */
        void usb::set_font_size(const int font_size)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::DISPLAY;
                cmd.display.identifier = varikey::display::FUNCTION::FONT;
                cmd.display.font = static_cast<varikey::display::FONT>(font_size);

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief print a text message on display
         *
         * set font size and cursor position (row, col) before print
         *
         * @param text
         */
        void usb::print_text(const char *text)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::DISPLAY;
                cmd.display.identifier = varikey::display::FUNCTION::TEXT;
                const size_t text_size = strnlen(text, varikey::display::MAX_TEXT_SIZE);
                memset(cmd.display.text, 0, varikey::display::MAX_TEXT_SIZE);
                memcpy((char *)cmd.display.text, text, text_size);
                cmd.display.text[text_size] = 0;

                if (send_report(cmd, 3 + varikey::display::MAX_TEXT_SIZE) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief get gadget processor temperature
         *
         * @return float
         */
        float usb::get_temperature()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                get_report_t cmd;
                cmd.report = varikey::GET_REPORT::TEMPERATURE;
                cmd.result = varikey::RESULT::SUCCESS;
                cmd.temperature.function = varikey::temperature::FUNCTION::GET;
                cmd.temperature.value = -1;

                if (send_report(cmd, 1 + 2 + sizeof(varikey::temperature::content_t)) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    return (cmd.temperature.value);
                }
            }

            return (float)0xffff;
        }

        /**
         * \brief set gadget state
         *
         * \param mode 0=MOUNT, 1=UNMOUNT, 2=SUSPEND and 3=RESUME
         */
        void usb::set_gadget(const uint8_t mode)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::GADGET;

                switch (mode)
                {
                case 0:
                    cmd.gadget.command = varikey::gadget::COMMAND::MOUNT;
                    break;
                case 1:
                    cmd.gadget.command = varikey::gadget::COMMAND::UNMOUNT;
                    break;
                case 2:
                    cmd.gadget.command = varikey::gadget::COMMAND::SUSPEND;
                    break;
                case 3:
                    cmd.gadget.command = varikey::gadget::COMMAND::RESUME;
                    break;
                default:
                    break;
                }

                if (send_report(cmd, 3) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief simulate key code
         *
         * \param mode
         */
        void usb::send_keycode(const uint8_t code)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::KEYPAD;
                cmd.keypad.identifier = varikey::keypad::IDENTIFIER::KEYCODE;
                cmd.keypad.function = varikey::keypad::FUNCTION::CLICK;
                cmd.keypad.code = code;

                if (send_report(cmd, 5) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief Enable/disable keypad events over HIC and HID interfaces
         *
         * \param interface 0=HCI, 1=HID
         * \param enable true/false
         */
        void usb::enable_interface(const uint8_t interface, const bool enable)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::KEYPAD;
                if (interface == 0)
                    cmd.keypad.identifier = varikey::keypad::IDENTIFIER::HCI;
                else if (interface == 1)
                    cmd.keypad.identifier = varikey::keypad::IDENTIFIER::HID;
                else
                    cmd.keypad.identifier = varikey::keypad::IDENTIFIER::UNDEFINED;
                if (enable)
                    cmd.keypad.function = varikey::keypad::FUNCTION::ENABLE;
                else
                    cmd.keypad.function = varikey::keypad::FUNCTION::DISABLE;

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief Set keypad mapping
         *
         * \param map 0=NUMBER, 1=FUNCTIONAL, 2=NAVIGATION, 3=TELEFON, 4=MULTIMEDIA, 5=CUSTOM
         */
        void usb::set_mapping(const uint8_t map)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::KEYPAD;
                cmd.keypad.identifier = varikey::keypad::IDENTIFIER::MAPPING;
                cmd.keypad.function = varikey::keypad::FUNCTION::SET;

                if (map >= 0 && map <= 5)
                    cmd.keypad.table = static_cast<varikey::keypad::TABLE>(map);
                else
                    cmd.keypad.table = varikey::keypad::TABLE::UNDEFINED;

                if (send_report(cmd, 5) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief
         *
         * \return float
         */
        uint8_t usb::get_mapping()
        {
            get_report_t cmd;
            cmd.report = varikey::GET_REPORT::MAPPING;
            cmd.result = varikey::RESULT::SUCCESS;
            cmd.keypad.identifier = varikey::keypad::IDENTIFIER::MAPPING;
            cmd.keypad.function = varikey::keypad::FUNCTION::GET;
            cmd.keypad.table = varikey::keypad::TABLE::UNDEFINED;

            if (device_handle != INVALID_HANDLE_VALUE)
            {
                if (send_report(cmd, 1 + 2 + sizeof(varikey::keypad::content_t)) < 0)
                {
                    cmd.keypad.table = varikey::keypad::TABLE::UNDEFINED;
                    perror("send_report failed");
                    usb_close();
                }
            }

            return (int)cmd.keypad.table;
        }

        /**
         * \brief Reset internal keypad mapping states
         */
        void usb::clean_mapping()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::KEYPAD;
                cmd.keypad.identifier = varikey::keypad::IDENTIFIER::MAPPING;
                cmd.keypad.function = varikey::keypad::FUNCTION::CLEAN;

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        void usb::set_output(const uint8_t _pin, const bool _value)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::GPIO;
                cmd.gpio.function = varikey::gpio::FUNCTION::DIRECTION_SET;
                switch (_pin)
                {
                case 0:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 1:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 2:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 3:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;

                default:
                    perror("wrong pin identifier");
                    return;
                }

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        const bool usb::is_output(const uint8_t _pin)
        {
            set_report_t request;
            request.command = varikey::COMMAND::CUSTOM;
            request.report = varikey::SET_REPORT::GPIO;
            request.gpio.function = varikey::gpio::FUNCTION::DIRECTION_GET;
            switch (_pin)
            {
            case 0:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                break;
            case 1:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN2;
                break;
            case 2:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN3;
                break;
            case 3:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN4;
                break;
            default:
                perror("wrong pin identifier");
                return false;
            }
            request.gpio.direction = varikey::gpio::DIRECTION::UNDEFINED;

            get_report_t response;
            response.report = varikey::GET_REPORT::GPIO;
            response.result = varikey::RESULT::SUCCESS;

            if (device_handle != INVALID_HANDLE_VALUE)
            {
                if (send_report(request, 5) < 0)
                {
                    response.gpio.function = varikey::gpio::FUNCTION::UNDEFINED;
                    perror("send_report failed");
                    usb_close();
                }
                else if (send_report(response, 1 + 2 + 3) < 0)
                {
                    response.gpio.function = varikey::gpio::FUNCTION::UNDEFINED;
                    perror("send_report failed");
                    usb_close();
                }
            }

            if (request.report == varikey::SET_REPORT::GPIO &&
                request.gpio.function == response.gpio.function &&
                request.gpio.identifier == response.gpio.identifier)
            {
                return response.gpio.direction == varikey::gpio::DIRECTION::OUTPUT ? true : false;
            }

            return false;
        }

        void usb::set_high(const uint8_t _pin, const bool _level)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::GPIO;
                cmd.gpio.function = varikey::gpio::FUNCTION::LEVEL_SET;
                switch (_pin)
                {
                case 0:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 1:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 2:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 3:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;

                default:
                    perror("wrong pin identifier");
                    return;
                }
                cmd.gpio.state = _level ? varikey::gpio::LEVEL::HIGH : varikey::gpio::LEVEL::LOW;

                if (send_report(cmd, 5) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        const bool usb::is_high(const uint8_t _pin)
        {
            set_report_t request;
            request.command = varikey::COMMAND::CUSTOM;
            request.report = varikey::SET_REPORT::GPIO;
            request.gpio.function = varikey::gpio::FUNCTION::LEVEL_GET;
            switch (_pin)
            {
            case 0:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                break;
            case 1:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                break;
            case 2:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                break;
            case 3:
                request.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                break;

            default:
                perror("wrong pin identifier");
                return false;
            }
            request.gpio.state = varikey::gpio::LEVEL::UNDEFINED;

            get_report_t response;
            response.report = varikey::GET_REPORT::GPIO;
            response.result = varikey::RESULT::SUCCESS;
            response.gpio.function = varikey::gpio::FUNCTION::UNDEFINED;
            response.gpio.identifier = varikey::gpio::IDENTIFIER::UNDEFINED;
            response.gpio.state = varikey::gpio::LEVEL::UNDEFINED;

            if (device_handle != INVALID_HANDLE_VALUE)
            {
                if (send_report(request, 5) < 0)
                {
                    request.gpio.state = varikey::gpio::LEVEL::UNDEFINED;
                    perror("send_report failed");
                    usb_close();
                }
                else if (send_report(request, 1 + 2 + 3) < 0)
                {
                    request.gpio.state = varikey::gpio::LEVEL::UNDEFINED;
                    perror("send_report failed");
                    usb_close();
                }
            }

            if (request.report == varikey::SET_REPORT::GPIO &&
                request.gpio.function == response.gpio.function &&
                request.gpio.identifier == response.gpio.identifier)
            {
                return response.gpio.state == varikey::gpio::LEVEL::HIGH ? true : false;
            }

            return false;
        }

        void usb::set_alarm(const uint8_t _pin, const bool _alarm)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::GPIO;
                if (_alarm)
                    cmd.gpio.function = varikey::gpio::FUNCTION::ENABLE;
                else
                    cmd.gpio.function = varikey::gpio::FUNCTION::DISABLE;
                switch (_pin)
                {
                case 0:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 1:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 2:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;
                case 3:
                    cmd.gpio.identifier = varikey::gpio::IDENTIFIER::PIN1;
                    break;

                default:
                    perror("wrong pin identifier");
                    return;
                }

                if (send_report(cmd, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        void usb::get_parameter(const uint8_t _identifier, chunk_t &_chunk)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                varikey::parameter::IDENTIFIER identifier = parameter::int2id(_identifier);
                size_t size = parameter::size(identifier);

                set_report_t request;
                request.command = varikey::COMMAND::CUSTOM;
                request.report = varikey::SET_REPORT::PARAMETER;
                request.parameter.function = varikey::parameter::FUNCTION::GET;
                request.parameter.identifier = identifier;

                get_report_t response;
                response.report = varikey::GET_REPORT::PARAMETER;
                response.result = varikey::RESULT::SUCCESS;
                response.parameter.function = varikey::parameter::FUNCTION::GET;
                response.parameter.identifier = identifier;

                if (send_report(request, 4) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
                else if (send_report(response, 4 + size) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }

                if (request.report == varikey::SET_REPORT::PARAMETER &&
                    request.gpio.function == response.gpio.function &&
                    request.gpio.identifier == response.gpio.identifier)
                {
                    _chunk.size = size;
                    memcpy(_chunk.space, response.parameter.buffer, _chunk.size);
                }
            }
        }

        void usb::set_parameter(const uint8_t _identifier, uint8_t const *const _space)
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                varikey::parameter::IDENTIFIER identifier = parameter::int2id(_identifier);
                size_t size = parameter::size(identifier);

                set_report_t cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.report = varikey::SET_REPORT::PARAMETER;
                cmd.parameter.function = varikey::parameter::FUNCTION::SET;
                cmd.parameter.identifier = identifier;

                memcpy(cmd.parameter.buffer, _space, size);

                if (send_report(cmd, 4 + size) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
            }
        }

        /**
         * \brief get varikey gadget serial number
         */
        bool usb::usb_get_serial()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                get_report_t cmd;
                cmd.report = varikey::GET_REPORT::SERIAL;
                cmd.result = varikey::RESULT::SUCCESS;

                const size_t SIZE = 1 + 2 +
                                    varikey::SERIAL_NUMBER_SIZE;

                if (send_report(cmd, SIZE) < 0)
                {
                    memset((char *)&cmd.identity.serial[0], 0xaa, varikey::SERIAL_NUMBER_SIZE);
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    memcpy((char *)device.serial, (char *)&cmd.identity.serial[0], varikey::SERIAL_NUMBER_SIZE);
                    return true;
                }
            }
            return false;
        }

        /**
         * \brief get varikey gadget unique identifier
         */
        bool usb::usb_get_unique()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                get_report_t cmd;
                cmd.report = varikey::GET_REPORT::UNIQUE;
                cmd.result = varikey::RESULT::SUCCESS;
                const size_t SIZE = 1 + 2 +
                                    sizeof(cmd.identity.unique);

                if (send_report(cmd, SIZE) < 0)
                {
                    cmd.identity.unique = 0;
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    device.unique = cmd.identity.unique;
                    return true;
                }
            }
            return false;
        }

        /**
         * \brief get varikey gadget hardware revision
         */
        bool usb::usb_get_hardware()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                get_report_t cmd;
                cmd.report = varikey::GET_REPORT::HARDWARE;
                cmd.result = varikey::RESULT::SUCCESS;
                const size_t SIZE = 1 + 2 +
                                    sizeof(cmd.identity.hardware);

                if (send_report(cmd, SIZE) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    uint8_t const *ptr = cmd.identity.buffer;
                    device.maintainer = deserialize_word(&ptr);
                    device.hardware = deserialize_word(&ptr);
                    device.number = *ptr++;
                    device.variant = *ptr++;
                    return true;
                }
            }
            return false;
        }

        /**
         * \brief get varikey gadget firmware revision
         */
        bool usb::usb_get_firmware()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                get_report_t cmd;
                cmd.report = varikey::GET_REPORT::FIRMWARE;
                cmd.result = varikey::RESULT::SUCCESS;
                const size_t SIZE = 1 + 2 +
                                    sizeof(varikey::identity::content_t::firmware);

                if (send_report(cmd, SIZE) < 0)
                {
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    uint8_t const *ptr = cmd.identity.buffer;
                    device.firmware = deserialize_word(&ptr);
                    device.revision = deserialize_word(&ptr);
                    device.patch = deserialize_word(&ptr);
                    device.build = deserialize_word(&ptr);
                    return true;
                }
            }
            return false;
        }

        /**
         * \brief send usb command report to the varikey gadget
         *
         * @param cmd
         * @return int
         */
        int usb::send_report(set_report_t &cmd, const size_t cmd_size)
        {
            assert(cmd_size <= sizeof(cmd));

            int result = -1;
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                if ((result = ioctl(device_handle, HIDIOCSOUTPUT(cmd_size), (void *)&cmd)) != (int)cmd_size)
                {
                    if (VERBOSE_OUTPUT)
                        perror("error sending output report");
                }
            }
            return result;
        }

        /**
         * \brief send usb feature report to the varikey gadget
         *
         * @param cmd
         * @return int
         */
        int usb::send_report(get_report_t &cmd, const size_t cmd_size)
        {
            assert(cmd_size <= sizeof(cmd));
            int result = -1;
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                if ((result = ioctl(device_handle, HIDIOCGFEATURE(cmd_size), (void *)&cmd)) != (int)cmd_size)
                {
                    if (VERBOSE_OUTPUT)
                        perror("error sending feature report");
                }
            }
            return result;
        }
    }
}
