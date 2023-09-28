/**
 * \file wizard.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <string>

#include "wizard_args.hpp"
#include "wizard_usb.hpp"

static void reset_device(wizard::usb &, const uint32_t unique);
static void set_position(wizard::usb &, const uint32_t unique, const uint8_t line, const uint8_t column);
static void draw_icon(wizard::usb &, const uint32_t unique, const uint8_t icon);
static void set_font_size(wizard::usb &, const uint32_t unique, const uint8_t font_size);
static void print_text(wizard::usb &, const uint32_t unique, const std::string &text);
static void get_temperature(wizard::usb &, const uint32_t unique);
static void set_backlight(wizard::usb &, const uint32_t unique, const uint8_t mode);

int main(int argc, char *argv[])
{
	wizard::arguments arguments;
	wizard_arguments_init(arguments);
	wizard_arguments_parse(arguments, argc, argv);

	static const bool VERBOSE_OUTPUT = arguments.verbose;

	wizard::usb wizard_usb_object;

	if (VERBOSE_OUTPUT)
		std::cout << "start " << argv[0] << std::endl;

	if (arguments.device != nullptr)
	{
		if (VERBOSE_OUTPUT)
			std::cout << "scan devices" << std::endl;

		wizard_usb_object.scan_devices(arguments.device);
	}

	if (arguments.reset != false)
	{
		reset_device(wizard_usb_object, arguments.unique);
	}
	else
	{
		if (arguments.temperature != false)
		{
			get_temperature(wizard_usb_object, arguments.unique);
		}

		if (arguments.backlight != 0xff)
		{
			set_backlight(wizard_usb_object, arguments.unique, arguments.backlight);
		}

		if (arguments.list != false)
		{
			std::cout << "list devices" << std::endl;
			wizard_usb_object.list_devices();
		}

		if (arguments.column != 0xff && arguments.line != 0xff)
		{
			set_position(wizard_usb_object, arguments.unique, arguments.line, arguments.column);
		}
		else if (!(arguments.column == 0xff && arguments.line == 0xff))
		{
			std::cout << "needs row and column values to set position" << std::endl;
		}

		if (arguments.icon != 0xff)
		{
			draw_icon(wizard_usb_object, arguments.unique, arguments.icon);
		}
		else if (arguments.text != nullptr)
		{
			if (arguments.unique != 0)
			{
				if (arguments.font_size != 0xff)
				{
					set_font_size(wizard_usb_object, arguments.unique, arguments.font_size);
				}

				print_text(wizard_usb_object, arguments.unique, arguments.text);
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