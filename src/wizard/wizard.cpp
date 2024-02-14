/**
 * \file wizard.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "wizard.hpp"
#include "wizard_args.hpp"
#include "wizard_config.hpp"
#include "wizard_usb.hpp"

bool VERBOSE_OUTPUT = true;

void hex_string_to_bytes(char **const _buffer, chunk_t &_chunk)
{
	assert(_chunk.space != nullptr);

	size_t counter = 0;
	char **ptr = _buffer;
	while (*ptr != nullptr)
	{
		const std::string arg = *ptr++;

		std::string hex_token;
		std::stringstream ss(arg);
		while (ss >> hex_token)
		{
			unsigned int hex_value;
			std::stringstream(hex_token) >> std::hex >> hex_value;

			std::cout << std::hex << hex_value << ' ';

			assert(counter < _chunk.size);
			_chunk.space[counter++] = hex_value;
		}
	}
	_chunk.size = counter;
}

static varikey::gadget::usb &open_device(wizard::usb &wizard_usb_object, const uint32_t unique);
static void close_device(wizard::usb &wizard_usb_object, varikey::gadget::usb &gadget);

static void reset_device(varikey::gadget::usb &);

static float get_temperature(varikey::gadget::usb &);

static void set_backlight(varikey::gadget::usb &,
						  const uint8_t mode);
static void set_backlight_color(varikey::gadget::usb &,
								const uint8_t mode,
								const uint8_t lr, const uint8_t lg, const uint8_t lb,
								const uint8_t rr, const uint8_t rg, const uint8_t rb);

static void clean_display(varikey::gadget::usb &);
static void set_position(varikey::gadget::usb &, const uint8_t line, const uint8_t column);
static void set_font_size(varikey::gadget::usb &, const uint8_t font_size);
static void print_text(varikey::gadget::usb &, const std::string &text);
static void draw_icon(varikey::gadget::usb &, const uint8_t icon);

static void set_gadget_status(varikey::gadget::usb &, const uint8_t);

static void send_keycode(varikey::gadget::usb &, const uint8_t);
static void enable_interface(varikey::gadget::usb &, const uint8_t, const bool);
static void set_mapping(varikey::gadget::usb &, const uint8_t);
static uint8_t get_mapping(varikey::gadget::usb &);
static void clean_mapping(varikey::gadget::usb &);

static void set_pin_direction(varikey::gadget::usb &, const uint8_t, const wizard::DIRECTION);
static bool get_pin_direction(varikey::gadget::usb &, const uint8_t);
static void set_pin_level(varikey::gadget::usb &, const uint8_t, const wizard::LEVEL);
static bool get_pin_level(varikey::gadget::usb &, const uint8_t);
static void set_pin_alarm(varikey::gadget::usb &, const uint8_t, wizard::ALARM);

static void get_parameter(varikey::gadget::usb &, const uint8_t);
static void set_parameter(varikey::gadget::usb &, const uint8_t, uint8_t const *const);

int main(int argc, char *argv[])
{
	wizard::config::load();

	wizard::arguments arguments;
	wizard_arguments_init(arguments);

	const size_t param_args_buffer_size = (argc + 1);
	arguments.strings = (char **)malloc(param_args_buffer_size * sizeof(char *));
	for (size_t i = 0; i < param_args_buffer_size; ++i)
	{
		arguments.strings[i] = nullptr;
	}

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
		varikey::gadget::usb &gadget = open_device(wizard_usb_object, serial_number);
		reset_device(gadget);
		close_device(wizard_usb_object, gadget);
	}
	else
	{
		wizard::usb wizard_usb_object;
		wizard_usb_object.scan_devices(device_path,
									   arguments.pid, arguments.vid);

		varikey::gadget::usb &gadget = open_device(wizard_usb_object, serial_number);
		if (arguments.temperature != false)
		{
			float value = get_temperature(gadget);
			std::cout << "device " << serial_number << " temperature " << value << std::endl;
		}

		if (arguments.backlight == (int)varikey::backlight::PROGRAM::MORPH ||
			arguments.backlight == (int)varikey::backlight::PROGRAM::SET)
		{
			set_backlight_color(gadget,
								arguments.backlight,
								arguments.r_left_value, arguments.g_left_value, arguments.b_left_value,
								arguments.r_right_value, arguments.g_right_value, arguments.b_right_value);
		}
		else if (arguments.backlight != 0xff)
		{
			set_backlight(gadget,
						  arguments.backlight);
		}

		if (arguments.column != 0xff && arguments.line != 0xff)
		{
			set_position(gadget, arguments.line, arguments.column);
		}
		else if (!(arguments.column == 0xff && arguments.line == 0xff))
		{
			std::cout << "needs row and column values to set position" << std::endl;
		}

		if (arguments.clean)
		{
			clean_display(gadget);
		}

		if (arguments.icon != 0xff)
		{
			draw_icon(gadget, arguments.icon);
		}
		else if (arguments.text != nullptr)
		{
			if (serial_number != 0)
			{
				if (arguments.font_size != 0xff)
				{
					set_font_size(gadget, arguments.font_size);
				}

				print_text(gadget, arguments.text);
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
				set_gadget_status(gadget, arguments.gadget_state);
			}
		}

		if (arguments.key_code != 0xff)
		{
			send_keycode(gadget, arguments.key_code);
		}

		if (arguments.enable_hci == true)
		{
			enable_interface(gadget, 0, arguments.enable_hci_value);
		}

		if (arguments.enable_hid == true)
		{
			enable_interface(gadget, 1, arguments.enable_hid_value);
		}

		if (arguments.clean_mapping)
		{
			clean_mapping(gadget);
		}

		if (arguments.mapping == (int)varikey::keycode::TABLE::CUSTOM ||
			arguments.mapping == (int)varikey::keycode::TABLE::FUNCTIONAL ||
			arguments.mapping == (int)varikey::keycode::TABLE::MULTIMEDIA ||
			arguments.mapping == (int)varikey::keycode::TABLE::NAVIGATION ||
			arguments.mapping == (int)varikey::keycode::TABLE::NUMBER ||
			arguments.mapping == (int)varikey::keycode::TABLE::TELEFON)
		{
			set_mapping(gadget, arguments.mapping);
		}
		else if (arguments.mapping == 0x77)
		{
			uint8_t value = get_mapping(gadget);
			std::cout << "device " << serial_number << " mapping " << (int)value << std::endl;
		}

		if (arguments.direction == wizard::DIRECTION::DIRECTION_IN ||
			arguments.direction == wizard::DIRECTION::DIRECTION_OUT)
		{
			set_pin_direction(gadget, arguments.pin, arguments.direction);
		}
		if (arguments.level == wizard::LEVEL::LEVEL_HIGH ||
			arguments.level == wizard::LEVEL::LEVEL_LOW)
		{
			set_pin_level(gadget, arguments.pin, arguments.level);
		}
		if (arguments.alarm == wizard::ALARM::ALARM_ENABLE ||
			arguments.alarm == wizard::ALARM::ALARM_DISABLE)
		{
			set_pin_alarm(gadget, arguments.pin, arguments.alarm);
		}
		if (arguments.get_direction)
		{
			bool value = get_pin_direction(gadget, arguments.pin);
			std::cout << "device " << serial_number << " pin  " << (int)arguments.pin << " is configured as " << (value ? "output" : "input") << std::endl;
		}
		if (arguments.get_level)
		{
			bool value = get_pin_level(gadget, arguments.pin);
			std::cout << "device " << serial_number << " pin  " << (int)arguments.pin << " level is " << (value ? "high" : "low") << std::endl;
		}

		if (arguments.parameter != 0xff)
		{
			if (arguments.parameter_function == wizard::FUNCTION::FUNCTION_GET)
			{
				get_parameter(gadget, arguments.parameter);
			}
			else if (arguments.parameter_function == wizard::FUNCTION::FUNCTION_SET)
			{
				CHUNK(parameter_hex_array, 100);
				hex_string_to_bytes(arguments.strings, parameter_hex_array);
				set_parameter(gadget, arguments.parameter, parameter_hex_array.space);
			}
		}

		close_device(wizard_usb_object, gadget);
	}

	free(arguments.strings);

	return 0;
}

static varikey::gadget::usb &open_device(wizard::usb &wizard_usb_object, const uint32_t unique)
{
	return wizard_usb_object.open_device(unique);
}

static void close_device(wizard::usb &wizard_usb_object, varikey::gadget::usb &gadget)
{
	wizard_usb_object.close_device(gadget);
}

static void reset_device(varikey::gadget::usb &gadget)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.reset_device();
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void clean_display(varikey::gadget::usb &gadget)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.clean_display();
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_position(varikey::gadget::usb &gadget,
						 const uint8_t line, const uint8_t column)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_position(line, column);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_font_size(varikey::gadget::usb &gadget,
						  const uint8_t font_size)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_font_size(font_size);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void print_text(varikey::gadget::usb &gadget,
					   const std::string &text)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.print_text(text.c_str());
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void draw_icon(varikey::gadget::usb &gadget,
					  const uint8_t icon)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.draw_icon(icon);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static float get_temperature(varikey::gadget::usb &gadget)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		return gadget.get_temperature();
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
	return -1000.0;
}

static void set_backlight(varikey::gadget::usb &gadget,
						  const uint8_t mode)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_backlight_mode(mode);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_backlight_color(varikey::gadget::usb &gadget,
								const uint8_t mode,
								const uint8_t lr, const uint8_t lg, const uint8_t lb,
								const uint8_t rr, const uint8_t rg, const uint8_t rb)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_backlight_color(mode, lr, lg, lb, rr, rg, rb);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_gadget_status(varikey::gadget::usb &gadget,
							  const uint8_t status)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_gadget(status);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void send_keycode(varikey::gadget::usb &gadget,
						 const uint8_t key_code)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.send_keycode(key_code);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void enable_interface(varikey::gadget::usb &gadget,
							 const uint8_t interface, const bool enable)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.enable_interface(interface, enable);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_mapping(varikey::gadget::usb &gadget,
						const uint8_t mapping)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_mapping(mapping);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static uint8_t get_mapping(varikey::gadget::usb &gadget)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		return gadget.get_mapping();
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
	return 0xff;
}

static void clean_mapping(varikey::gadget::usb &gadget)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.clean_mapping();
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_pin_direction(varikey::gadget::usb &gadget,
							  const uint8_t pin, const wizard::DIRECTION direction)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_output(pin, direction == wizard::DIRECTION::DIRECTION_OUT ? true : false);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static bool get_pin_direction(varikey::gadget::usb &gadget,
							  const uint8_t pin)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		return gadget.is_output(pin);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
	return false;
}

static void set_pin_level(varikey::gadget::usb &gadget,
						  const uint8_t pin, const wizard::LEVEL level)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_high(pin, level == wizard::LEVEL::LEVEL_HIGH ? true : false);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static bool get_pin_level(varikey::gadget::usb &gadget,
						  const uint8_t pin)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		return gadget.is_high(pin);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
	return false;
}

static void set_pin_alarm(varikey::gadget::usb &gadget,
						  const uint8_t pin, wizard::ALARM alarm)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_alarm(pin, alarm == wizard::ALARM::ALARM_ENABLE ? true : false);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void get_parameter(varikey::gadget::usb &gadget,
						  const uint8_t _parameter)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		uint8_t space[128];
		chunk_t chunk = {.space = space, .size = 128};

		gadget.get_parameter(_parameter, chunk);
		std::cout << "parameter " << (int)_parameter << " value: [";
		for (size_t i = 0; i < chunk.size; ++i)
		{
			std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)chunk.space[i] << " ";
		}
		std::cout << "]" << std::endl;
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}

static void set_parameter(varikey::gadget::usb &gadget,
						  const uint8_t _parameter, uint8_t const *const _space)
{
	if (gadget.is_valid() && gadget.is_open())
	{
		gadget.set_parameter(_parameter, _space);
	}
	else
	{
		std::cout << "invalid device" << std::endl;
	}
}
