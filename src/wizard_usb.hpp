/**
 * \file wizard_usb.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __WIZARD_USB_HPP__
#define __WIZARD_USB_HPP__

#include <list>
#include <string>

#include "varikey_command.hpp"
#include "varikey_device.hpp"
#include "varikey_gadget_usb.hpp"

namespace wizard
{
	class usb
	{
	public:
		usb();
		virtual ~usb();

		int scan_devices(const std::string &);

		varikey::gadget::usb &open_device(const uint32_t);
		void close_device(varikey::gadget::usb &);

		void list_devices();

	private:
		struct device_descriptor
		{
			std::string device_path;
			varikey::gadget::usb device;
		};

		std::list<device_descriptor> descriptor;

		const device_descriptor &find_valid_unique(const uint32_t) const;
	};
}

#endif // __WIZARD_USB_HPP__
