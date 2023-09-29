/**
 * \file varikey_gadget_usb.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/hiddev.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <unistd.h>

#include "varikey_gadget_usb.hpp"

/**
 * \brief USB device identifiers
 *
 * 0xcafe=51966, 0x4004=16388
 * attention: devices are filtered by product:vendor values
 * @{
 */
#define VARIKEY_VENDOR_IDENTIFIER 0xcafe
#define VARIKEY_PRODUCT_IDENTIFIER 0x4004
/** }@ */

namespace varikey
{
    namespace gadget
    {
        /**
         * \brief Construct a new wizard usb entity::wizard usb entity object
         */
        usb::usb() {}

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
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
            }

            device_handle = open(_device_path, O_RDWR);
            if (device_handle < 0)
            {
                device_handle = INVALID_HANDLE_VALUE;
                return;
            }

            device_valid = true;

            struct hidraw_devinfo dinfo;
            if (ioctl(device_handle, HIDIOCGRAWINFO, (void *)&dinfo) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
                return;
            }

            if (!(((uint16_t)dinfo.vendor == VARIKEY_VENDOR_IDENTIFIER) &&
                  ((uint16_t)dinfo.product == VARIKEY_PRODUCT_IDENTIFIER)))
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
                return;
            }

            device.bustype = dinfo.bustype;
            device.product = dinfo.product;
            device.vendor = dinfo.vendor;

            if (ioctl(device_handle, HIDIOCGRAWNAME(VARIKEY_NAME_SIZE), (void *)&device.name) < 0)
            {
                perror("error sending report");
                fprintf(stderr, "error sending report: %d %s\n", errno, strerror(errno));
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
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                usb_get_serial();
                usb_get_unique();
                usb_get_gadget();
                usb_get_hardware();
                usb_get_version();
            }
        }

        /**
         * \brief reset and close an open device
         */
        void usb::reset_device()
        {
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::RESET);

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
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
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::POSITION);
            cmd.payload.position.line = line;
            cmd.payload.position.column = column;

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
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
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::ICON);
            cmd.payload.byte_value = icon;

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
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
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::FONT_SIZE);
            cmd.payload.byte_value = font_size;

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
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
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::TEXT);
            memset(cmd.payload.text, 0, sizeof(cmd.payload.text));
            memcpy((char *)cmd.payload.text, text, strlen(text));

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
            }
        }

        /**
         * \brief set backlight mode for varikey gadget
         *
         * check varikey documentation for available modes
         *
         * @param mode
         */
        void usb::set_backlight_mode(const int mode)
        {
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::BACKLIGHT);
            cmd.payload.byte_value = mode;

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
            }
        }

        /**
         * \brief set backlight color for varikey gadget
         * 
         * \param r red channel
         * \param g green channel
         * \param b blue channel
         */
        void usb::set_backlight_color(const uint8_t r, const uint8_t g, const uint8_t b)
        {
            command cmd;
            cmd.report = static_cast<unsigned char>(varikey::report_id::CUSTOM);
            cmd.command = static_cast<unsigned char>(varikey::command_id::BACKLIGHT);
            cmd.payload.text[0] = 0xaa;
            cmd.payload.text[1] = r;
            cmd.payload.text[2] = g;
            cmd.payload.text[3] = b;

            if (send_report(device_handle, cmd) < 0)
            {
                close(device_handle);
                device_handle = INVALID_HANDLE_VALUE;
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
                cmd.report = static_cast<unsigned char>(varikey::report_id::TEMPERATURE);

                if (send_report(device_handle, cmd) >= 0)
                {
                    return (cmd.payload.long_value / 1000.0);
                }
            }

            return (float)0xffff;
        }

        /**
         * \brief get varikey gadget serial number
         */
        void usb::usb_get_serial()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = static_cast<unsigned char>(varikey::report_id::SERIAL);

                if (send_report(device_handle, cmd) >= 0)
                {
                    memcpy((char *)device.serial, (char *)&cmd.payload.serial[0], sizeof(device.serial));
                }
            }
        }

        /**
         * \brief get varikey gadget unique identifier
         */
        void usb::usb_get_unique()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = static_cast<unsigned char>(varikey::report_id::UNIQUE);

                if (send_report(device_handle, cmd) >= 0)
                {
                    device.unique = cmd.payload.long_value;
                }
            }
        }

        /**
         * \brief get varikey gadget type
         */
        void usb::usb_get_gadget()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = static_cast<unsigned char>(varikey::report_id::GADGET);

                if (send_report(device_handle, cmd) >= 0)
                {
                    device.gadget = (gadget::type)cmd.payload.byte_value;
                }
            }
        }

        /**
         * \brief get varikey gadget hardware revision
         */
        void usb::usb_get_hardware()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = static_cast<unsigned char>(varikey::report_id::HARDWARE);
                if (send_report(device_handle, cmd) >= 0)
                {
                    device.hardware = cmd.payload.long_value;
                }
            }
        }

        /**
         * \brief get varikey gadget firmware revision
         */
        void usb::usb_get_version()
        {
            if (device_handle != INVALID_HANDLE_VALUE)
            {
                feature cmd;
                cmd.report = static_cast<unsigned char>(varikey::report_id::VERSION);

                if (send_report(device_handle, cmd) >= 0)
                {
                    device.version = cmd.payload.long_value;
                }
            }
        }

        /**
         * \brief send usb command report to the varikey gadget
         *
         * @param handle
         * @param cmd
         * @return int
         */
        int usb::send_report(const unsigned long int handle, command &cmd)
        {
            int result = -1;
            if ((result = ioctl(handle, HIDIOCSOUTPUT(sizeof(cmd)), (void *)&cmd)) < 0)
            {
                perror("error sending output report");
                fprintf(stderr, "error sending output report: %d %s\n", errno, strerror(errno));
            }
            return result;
        }

        /**
         * \brief send usb feature report to the varikey gadget
         *
         * @param handle
         * @param cmd
         * @return int
         */
        int usb::send_report(const unsigned long int handle, feature &cmd)
        {
            int result = -1;
            if ((result = ioctl(handle, HIDIOCGFEATURE(sizeof(cmd)), (void *)&cmd)) < 0)
            {
                perror("error sending feature report");
                fprintf(stderr, "error sending feature report: %d %s\n", errno, strerror(errno));
            }
            return result;
        }
    }
}