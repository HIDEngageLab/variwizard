/**
 * \file wizard_usb.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <iostream>
#include <sstream>

#include <linux/hiddev.h>
#include <linux/hidraw.h>
#include <linux/input.h>

#include "varikey_command.hpp"
#include "varikey_device.hpp"
#include "wizard_usb.hpp"

/**
 * @brief USB device identifiers
 *
 * 0xcafe=51966, 0x4004=16388
 * attention: devices are filtered by product:vendor values
 * @{
 */
#define VARIKEY_VENDOR_IDENTIFIER 0xcafe
#define VARIKEY_PRODUCT_IDENTIFIER 0x4004
/** }@ */

/**
 * @brief max number of expected hid devices for a scan
 * @{
 */
#define MAX_HID_NUMBER 16
/** }@ */

namespace wizard
{
	usb::usb() {}

	usb::~usb() {}

	/**
	 * @brief open all devices with names corresponds to device pattern
	 *
	 * @param _device_pattern example: /dev/hidraw for /dev/hidraw0... /dev/hidraw15
	 * @return int number of devices
	 */
	int usb::scan_devices(const std::string &_device_pattern)
	{
		for (int i = 0; i < MAX_HID_NUMBER; ++i)
		{
			std::stringstream device_name;
			device_name << _device_pattern << i;

			device_descriptor tmp;
			tmp.device.usb_open(device_name.str().c_str());

			if (tmp.device.is_open())
			{
				tmp.device_path = device_name.str();
				tmp.device.usb_init();
				tmp.device.usb_close();

				descriptor.push_back(tmp);
			}
		}

		return descriptor.size();
	}

	/**
	 * \brief open device
	 */
	varikey::gadget::usb &usb::open_device(const uint32_t _unique)
	{
		device_descriptor &descriptor = const_cast<device_descriptor &>(find_valid_unique(_unique));

		if (descriptor.device.is_valid() && !descriptor.device.is_open())
		{
			descriptor.device.usb_open(descriptor.device_path.c_str());
			if (descriptor.device.is_open())
			{
				return descriptor.device;
			}
		}

		static varikey::gadget::usb bad_choice;
		return bad_choice;
	}

	/**
	 * @brief close device
	 */
	void usb::close_device(varikey::gadget::usb &_device)
	{
		if (_device.is_valid() && _device.is_open())
		{
			_device.usb_close();
		}
	}

	const usb::device_descriptor &usb::find_valid_unique(const uint32_t _unique) const
	{
		for (auto const &i : descriptor)
		{
			if (i.device.is_valid() && i.device.get_unique() == _unique)
			{
				return i;
			}
		}

		static const device_descriptor bad_choice;
		return bad_choice;
	}

	void usb::list_devices()
	{
		for (auto &i : descriptor)
		{
			if (i.device.is_valid())
			{
				i.device.usb_open(i.device_path.c_str());
				if (i.device.is_open())
				{
					int unique = i.device.get_unique();
					int gadget = static_cast<int>(i.device.get_gadget());
					int hardware = i.device.get_hardware();
					int version = i.device.get_version();

					std::cout << std::hex << "unique 0x" << unique << "(" << std::dec << unique << ") ";
					std::cout << std::dec << "gadget " << gadget << " ";
					std::cout << std::hex << "hardware 0x" << hardware << "(" << std::dec << hardware << ") ";
					std::cout << std::hex << "version 0x" << version << "(" << std::dec << version << ") ";
					std::cout << i.device_path;
					std::cout << std::endl;

					i.device.usb_close();
				}
			}
		}
	}
}
