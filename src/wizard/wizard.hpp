/**
 * \file wizard.hpp
 * \author Koch, Roman (koch.roman@googlemail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __USB_WIZARD_HPP__
#define __USB_WIZARD_HPP__

extern bool VERBOSE_OUTPUT;

/**
 * @brief USB device identifiers
 *
 * 0xcafe=51966, 0x4004=16388
 * attention: devices are filtered by product:vendor values
 * @{
 */
const uint16_t VARIKEY_VENDOR_IDENTIFIER = 0xBEEF;
const uint16_t VARIKEY_PRODUCT_IDENTIFIER = 0x0024;
/** }@ */

#endif // __USB_WIZARD_HPP__
