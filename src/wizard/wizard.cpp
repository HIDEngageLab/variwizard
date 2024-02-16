/**
 * \file wizard.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "wizard.hpp"
#include "wizard_args.hpp"
#include "wizard_config.hpp"
#include "wizard_usb.hpp"

bool VERBOSE_OUTPUT = true;

static void reset_device(wizard::usb &, const uint32_t unique);
static void set_position(wizard::usb &, const uint32_t unique, const uint8_t line, const uint8_t column);
static void clean_display(wizard::usb &, const uint32_t unique);
static void draw_icon(wizard::usb &, const uint32_t unique, const uint8_t icon);
static void set_font_size(wizard::usb &, const uint32_t unique, const uint8_t font_size);
static void print_text(wizard::usb &, const uint32_t unique, const std::string &text);
static void get_temperature(wizard::usb &, const uint32_t unique);
static void set_backlight(wizard::usb &, const uint32_t unique, const uint8_t mode);
static void set_backlight_color(wizard::usb &, const uint32_t unique, const uint8_t mode,
								const uint8_t lr, const uint8_t lg, const uint8_t lb,
								const uint8_t rr, const uint8_t rg, const uint8_t rb);

int main(int argc, char *argv[])
{
	wizard::config::load();

	wizard::arguments arguments;
	wizard_arguments_init(arguments);
	wizard_arguments_parse(arguments, argc, argv);

	VERBOSE_OUTPUT = arguments.verbose;

	if (VERBOSE_OUTPUT)
		std::cout << "start " << argv[0] << std::endl;

	const std::string device_path = wizard::config::get_device();
	const uint32_t serial_number = wizard::config::get_serial();
	if (arguments.list != false)
	{
		wizard::usb wizard_usb_object;
		wizard_usb_object.scan_devices(arguments.devices_directory,
									   arguments.pid, arguments.vid);
		wizard_usb_object.list_devices();
	}
	else if (arguments.reset != false)
	{
		wizard::usb wizard_usb_object;
		wizard_usb_object.scan_devices(device_path,
									   arguments.pid, arguments.vid);
		reset_device(wizard_usb_object, serial_number);
	}
	else
	{
		wizard::usb wizard_usb_object;
		wizard_usb_object.scan_devices(device_path,
									   arguments.pid, arguments.vid);

		if (arguments.temperature != false)
		{
			get_temperature(wizard_usb_object, serial_number);
		}

		if (arguments.backlight == (int)varikey::backlight::PROGRAM::MORPH ||
			arguments.backlight == (int)varikey::backlight::PROGRAM::SET)
		{
			set_backlight_color(wizard_usb_object, serial_number,
								arguments.backlight,
								arguments.r_left_value, arguments.g_left_value, arguments.b_left_value,
								arguments.r_right_value, arguments.g_right_value, arguments.b_right_value);
		}
		else if (arguments.backlight != 0xff)
		{
			set_backlight(wizard_usb_object, serial_number, arguments.backlight);
		}

		if (arguments.column != 0xff && arguments.line != 0xff)
		{
			set_position(wizard_usb_object, serial_number, arguments.line, arguments.column);
		}
		else if (!(arguments.column == 0xff && arguments.line == 0xff))
		{
			std::cout << "needs row and column values to set position" << std::endl;
		}

		if (arguments.clean)
		{
			clean_display(wizard_usb_object, serial_number);
		}

		if (arguments.icon != 0xff)
		{
			draw_icon(wizard_usb_object, serial_number, arguments.icon);
		}
		else if (arguments.text != nullptr)
		{
			if (serial_number != 0)
			{
				if (arguments.font_size != 0xff)
				{
					set_font_size(wizard_usb_object, serial_number, arguments.font_size);
				}

				print_text(wizard_usb_object, serial_number, arguments.text);
			}
			else
			{
				std::cout << "needs unique identifier to print text" << std::endl;
			}
		}
	}

	return 0;
}

static void reset_device(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.reset_device();
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_position(wizard::usb &wizard_usb_object, const uint32_t unique,
						 const uint8_t line, const uint8_t column)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_position(line, column);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void clean_display(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.clean_display();
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void draw_icon(wizard::usb &wizard_usb_object, const uint32_t unique,
					  const uint8_t icon)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.draw_icon(icon);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_font_size(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t font_size)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_font_size(font_size);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void print_text(wizard::usb &wizard_usb_object, const uint32_t unique,
					   const std::string &text)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.print_text(text.c_str());
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_temperature(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		float value = gadget.get_temperature();
		std::cout << "device " << unique << " temperature " << value << std::endl;

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_backlight(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t mode)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_backlight_mode(mode);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_backlight_color(wizard::usb &wizard_usb_object, const uint32_t unique,
								const uint8_t mode,
								const uint8_t lr, const uint8_t lg, const uint8_t lb,
								const uint8_t rr, const uint8_t rg, const uint8_t rb)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_backlight_color(mode, lr, lg, lb, rr, rg, rb);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}
