/**
 * \file wizard_usb.cpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <linux/hiddev.h>
#include <linux/hidraw.h>
#include <linux/input.h>

#include "varikey_command.hpp"
#include "varikey_device.hpp"
#include "wizard.hpp"
#include "wizard_args.hpp"
#include "wizard_usb.hpp"

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
	int usb::scan_devices(const std::string &_device_pattern,
						  const uint16_t pid, const uint16_t vid)
	{
		if (VERBOSE_OUTPUT)
			std::cout << "scan devices" << std::endl;

		for (int i = 0; i < MAX_HID_NUMBER; ++i)
		{
			std::stringstream device_name;
			device_name << _device_pattern << i;

			device_descriptor tmp;
			tmp.device.usb_open(device_name.str().c_str());

			if (tmp.device.get_vendor() == vid && tmp.device.get_product() == pid)
			{
				if (tmp.device.is_open())
				{
					tmp.device_path = device_name.str();
					tmp.device.usb_init();
					tmp.device.usb_close();
					if (tmp.device.is_valid())
						descriptor.push_back(tmp);
				}
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
		if (descriptor.size() == 0)
		{
			std::cout << "no devices found" << std::endl;
		}
		else
		{
			std::cout << "list devices" << std::endl;
		}
		std::cout.width(25);
		std::cout << std::left << "unique";
		std::cout.width(25);
		std::cout << std::left << "hardware";
		std::cout.width(25);
		std::cout << std::left << "firmware";
		std::cout.width(8);
		std::cout << std::left << "pid";
		std::cout.width(8);
		std::cout << std::left << "vid";
		std::cout << std::left << "path";
		std::cout << std::endl;
		for (auto &i : descriptor)
		{
			if (i.device.is_valid())
			{
				i.device.usb_open(i.device_path.c_str());
				if (i.device.is_open())
				{
					unsigned int unique = i.device.get_unique();
					unsigned int hardware = i.device.get_hardware();
					unsigned int number = i.device.get_number();
					unsigned int variant = i.device.get_variant();
					unsigned int firmware = i.device.get_firmware();
					unsigned int revision = i.device.get_revision();
					unsigned int patch = i.device.get_patch();
					unsigned int pid = i.device.get_product();
					unsigned int vid = i.device.get_vendor();

					std::cout << "0x" << std::hex << std::right << std::setw(8);
					auto default_fill_value = std::cout.fill('0');
					std::cout << unique;
					std::cout.fill(default_fill_value);
					std::cout << "(" << std::dec << std::right << std::setw(12) << unique << ") ";

					std::stringstream hw;
					hw << std::dec << hardware << '.' << number << '/' << variant;
					std::cout << std::left << std::setw(24) << hw.str() << " ";

					std::stringstream fw;
					fw << firmware << '.' << revision << '.' << patch;
					std::cout << std::left << std::setw(24) << fw.str() << " ";

					std::cout << std::right << std::hex << "0x" << std::setfill('0') << std::setw(4) << pid << "  ";

					std::cout << std::right << std::hex << "0x" << std::setfill('0') << std::setw(4) << vid << "  ";

					std::cout << i.device_path;
					std::cout << std::endl;

					i.device.usb_close();
				}
			}
		}
	}
}
