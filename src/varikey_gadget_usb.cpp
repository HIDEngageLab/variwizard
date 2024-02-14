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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::RESET;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::BACKLIGHT;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::BACKLIGHT;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::DISPLAY;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::DISPLAY;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::DISPLAY;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::DISPLAY;
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
                command cmd;
                cmd.command = varikey::COMMAND::CUSTOM;
                cmd.identifier = varikey::IDENTIFIER::DISPLAY;
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
                feature cmd;
                cmd.report = varikey::REPORT::TEMPERATURE;

                if (send_report(cmd, 1 + sizeof(varikey::temperature::content_t)) < 0)
                {
                    cmd.temperature.value = -1;
                    perror("send_report failed");
                    usb_close();
                }
                else
                {
                    return (cmd.temperature.value / 1000.0);
                }
            }

            return (float)0xffff;
        }

        /**
         * \brief get varikey gadget serial number
         */
        bool usb::usb_get_serial()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = varikey::REPORT::SERIAL;

                const size_t SIZE = 1 +
                                    sizeof(cmd.report) +
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
                feature cmd;
                cmd.report = varikey::REPORT::UNIQUE;
                const size_t SIZE = 1 +
                                    sizeof(cmd.report) +
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
                feature cmd;
                cmd.report = varikey::REPORT::HARDWARE;
                const size_t SIZE = 1 +
                                    sizeof(cmd.report) +
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
                feature cmd;
                cmd.report = varikey::REPORT::FIRMWARE;
                const size_t SIZE = 1 +
                                    sizeof(varikey::identity::IDENTIFIER) +
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
        int usb::send_report(command &cmd, const size_t cmd_size)
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
        int usb::send_report(feature &cmd, const size_t cmd_size)
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