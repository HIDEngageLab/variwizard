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

            uint16_t get_product() const { return device.product; }
            uint16_t get_vendor() const { return device.vendor; }
            uint32_t get_unique() const { return device.unique; }
            uint16_t get_hardware() const { return device.hardware; }
            uint8_t get_number() const { return device.number; }
            uint8_t get_variant() const { return device.variant; }
            uint16_t get_firmware() const { return device.firmware; }
            uint16_t get_revision() const { return device.revision; }
            uint16_t get_patch() const { return device.patch; }
            uint16_t get_build() const { return device.build; }

            void reset_device();
            bool is_open() const;
            bool is_valid() const { return device_valid; }

            void clean_display();
            void set_position(const int line, const int column);
            void set_font_size(const int font_size);
            void print_text(const char *text);
            void draw_icon(const int icon);

            void set_backlight_mode(const uint8_t mode);
            void set_backlight_color(const uint8_t mode,
                                     const uint8_t lr, const uint8_t lg, const uint8_t lb,
                                     const uint8_t rr, const uint8_t rg, const uint8_t rb);

            float get_temperature();

            void set_gadget(const uint8_t mode);

            void send_keycode(const uint8_t mode);

            void enable_interface(const uint8_t interface, const bool enable);

            void set_mapping(const uint8_t map);
            uint8_t get_mapping();
            void clean_mapping();

        private:
            bool usb_get_serial();
            bool usb_get_unique();
            bool usb_get_hardware();
            bool usb_get_firmware();

            int send_report(command &cmd, const size_t);
            int send_report(feature &cmd, const size_t);

            varikey::device device;

            int device_handle;
            bool device_valid;
        };
    }
}

#endif /* __VARIKEY_GADGET_USB_HPP__ */
