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

static void reset_device(wizard::usb &, const uint32_t);

static void clean_display(wizard::usb &, const uint32_t);
static void set_position(wizard::usb &, const uint32_t, const uint8_t line, const uint8_t column);
static void set_font_size(wizard::usb &, const uint32_t, const uint8_t font_size);
static void print_text(wizard::usb &, const uint32_t, const std::string &text);
static void draw_icon(wizard::usb &, const uint32_t, const uint8_t icon);

static void get_temperature(wizard::usb &, const uint32_t);

static void set_backlight(wizard::usb &, const uint32_t, const uint8_t mode);
static void set_backlight_color(wizard::usb &, const uint32_t, const uint8_t mode,
								const uint8_t lr, const uint8_t lg, const uint8_t lb,
								const uint8_t rr, const uint8_t rg, const uint8_t rb);

static void set_gadget_status(wizard::usb &, const uint32_t, const uint8_t);

static void send_keycode(wizard::usb &, const uint32_t, const uint8_t);
static void enable_interface(wizard::usb &, const uint32_t, const uint8_t, const bool);
static void set_mapping(wizard::usb &, const uint32_t, const uint8_t);
static void get_mapping(wizard::usb &, const uint32_t);
static void clean_mapping(wizard::usb &, const uint32_t);

static void set_pin_direction(wizard::usb &, const uint32_t, const uint8_t, const wizard::DIRECTION);
static void get_pin_direction(wizard::usb &, const uint32_t, const uint8_t);
static void set_pin_level(wizard::usb &, const uint32_t, const uint8_t, const wizard::LEVEL);
static void get_pin_level(wizard::usb &, const uint32_t, const uint8_t);
static void set_pin_alarm(wizard::usb &, const uint32_t, const uint8_t, wizard::ALARM);

static void get_parameter(wizard::usb &, const uint32_t, const uint8_t);
static void set_parameter(wizard::usb &, const uint32_t, const uint8_t, uint8_t const *const);

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

		if (arguments.gadget_state != 0xff)
		{
			if (arguments.gadget_state == 0 ||
				arguments.gadget_state == 1 ||
				arguments.gadget_state == 2 ||
				arguments.gadget_state == 3)
			{
				set_gadget_status(wizard_usb_object, serial_number, arguments.gadget_state);
			}
		}

		if (arguments.key_code != 0xff)
		{
			send_keycode(wizard_usb_object, serial_number, arguments.key_code);
		}

		if (arguments.enable_hci == true)
		{
			enable_interface(wizard_usb_object, serial_number, 0, arguments.enable_hci_value);
		}

		if (arguments.enable_hid == true)
		{
			enable_interface(wizard_usb_object, serial_number, 1, arguments.enable_hid_value);
		}

		if (arguments.clean_mapping)
		{
			clean_mapping(wizard_usb_object, serial_number);
		}

		if (arguments.mapping == (int)varikey::keycode::TABLE::CUSTOM ||
			arguments.mapping == (int)varikey::keycode::TABLE::FUNCTIONAL ||
			arguments.mapping == (int)varikey::keycode::TABLE::MULTIMEDIA ||
			arguments.mapping == (int)varikey::keycode::TABLE::NAVIGATION ||
			arguments.mapping == (int)varikey::keycode::TABLE::NUMBER ||
			arguments.mapping == (int)varikey::keycode::TABLE::TELEFON)
		{
			set_mapping(wizard_usb_object, serial_number, arguments.mapping);
		}
		else if (arguments.mapping == 0x77)
		{
			get_mapping(wizard_usb_object, serial_number);
		}

		if (arguments.direction == wizard::DIRECTION::DIRECTION_IN ||
			arguments.direction == wizard::DIRECTION::DIRECTION_OUT)
		{
			set_pin_direction(wizard_usb_object, serial_number, arguments.pin, arguments.direction);
		}
		if (arguments.level == wizard::LEVEL::LEVEL_HIGH ||
			arguments.level == wizard::LEVEL::LEVEL_LOW)
		{
			set_pin_level(wizard_usb_object, serial_number, arguments.pin, arguments.level);
		}
		if (arguments.alarm == wizard::ALARM::ALARM_ENABLE ||
			arguments.alarm == wizard::ALARM::ALARM_DISABLE)
		{
			set_pin_alarm(wizard_usb_object, serial_number, arguments.pin, arguments.alarm);
		}
		if (arguments.get_direction)
		{
			get_pin_direction(wizard_usb_object, serial_number, arguments.pin);
		}
		if (arguments.get_level)
		{
			get_pin_level(wizard_usb_object, serial_number, arguments.pin);
		}

		if (arguments.parameter != 0xff)
		{
			if (arguments.parameter_function == wizard::FUNCTION::FUNCTION_GET)
			{
				get_parameter(wizard_usb_object, serial_number, arguments.parameter);
			}
			else if (arguments.parameter_function == wizard::FUNCTION::FUNCTION_SET)
			{
				set_parameter(wizard_usb_object, serial_number, arguments.parameter, arguments.parameter_value);
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

static void set_gadget_status(wizard::usb &wizard_usb_object, const uint32_t unique,
							  const uint8_t status)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_gadget(status);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void send_keycode(wizard::usb &wizard_usb_object, const uint32_t unique,
						 const uint8_t key_code)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.send_keycode(key_code);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void enable_interface(wizard::usb &wizard_usb_object, const uint32_t unique,
							 const uint8_t interface, const bool enable)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.enable_interface(interface, enable);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_mapping(wizard::usb &wizard_usb_object, const uint32_t unique,
						const uint8_t mapping)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_mapping(mapping);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_mapping(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		uint8_t value = gadget.get_mapping();
		std::cout << "device " << unique << " mapping " << (int)value << std::endl;

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void clean_mapping(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.clean_mapping();
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_pin_direction(wizard::usb &wizard_usb_object, const uint32_t unique,
							  const uint8_t pin, const wizard::DIRECTION direction)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_output(pin, direction == wizard::DIRECTION::DIRECTION_OUT ? true : false);
		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_pin_direction(wizard::usb &wizard_usb_object, const uint32_t unique,
							  const uint8_t pin)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		bool value = gadget.is_output(pin);
		std::cout << "device " << unique << " pin  " << (int)pin << " is configured as " << (value ? "output" : "input") << std::endl;

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_pin_level(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t pin, const wizard::LEVEL level)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_high(pin, level == wizard::LEVEL::LEVEL_HIGH ? true : false);

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_pin_level(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t pin)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		bool value = gadget.is_high(pin);
		std::cout << "device " << unique << " pin  " << pin << " level is " << (value ? "high" : "low") << std::endl;

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_pin_alarm(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t pin, wizard::ALARM alarm)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_alarm(pin, alarm == wizard::ALARM::ALARM_ENABLE ? true : false);

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_parameter(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t _parameter)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		chunk_t chunk;
		gadget.get_parameter(_parameter, chunk);
		std::cout << "parameter " << (int)_parameter << " ";
		for (size_t i = 0; i < chunk.size; ++i)
		{
			std::cout << (int)chunk.space[i] << " ";
		}
		std::cout << std::endl;

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_parameter(wizard::usb &wizard_usb_object, const uint32_t unique,
						  const uint8_t _parameter, uint8_t const *const _space)
{
	varikey::gadget::usb &gadget = wizard_usb_object.open_device(unique);
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_parameter(_parameter, _space);

		wizard_usb_object.close_device(gadget);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}
