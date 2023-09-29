/**
 * \file varikey_gadget_usb.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __VARIKEY_GADGET_USB_HPP__
#define __VARIKEY_GADGET_USB_HPP__

#include "varikey_command.hpp"
#include "varikey_device.hpp"

#define INVALID_HANDLE_VALUE 0xffff

namespace varikey
{
    namespace gadget
    {
        class usb
        {
        public:
            usb();
            virtual ~usb();

            void usb_open(const char *device_path);
            void usb_init();
            void usb_close();

            uint32_t get_unique() const { return device.unique; }
            gadget::type get_gadget() const { return device.gadget; }
            uint32_t get_hardware() const { return device.hardware; }
            uint32_t get_version() const { return device.version; }

            void reset_device();
            bool is_open() const { return device_handle != INVALID_HANDLE_VALUE; }
            bool is_valid() const { return device_valid; }

            void set_position(const int line, const int column);
            void draw_icon(const int icon);
            void set_font_size(const int font_size);
            void print_text(const char *text);
            void set_backlight_mode(const int mode);
            void set_backlight_color(const uint8_t r, const uint8_t g, const uint8_t b);

            float get_temperature();

        private:
            void usb_get_serial();
            void usb_get_unique();
            void usb_get_gadget();
            void usb_get_hardware();
            void usb_get_version();

            int send_report(const unsigned long int handle, command &cmd);
            int send_report(const unsigned long int handle, feature &cmd);

            varikey::device device;

            unsigned long int device_handle{INVALID_HANDLE_VALUE};
            bool device_valid{false};
        };
    }
}

#endif /* __VARIKEY_GADGET_USB_HPP__ */
