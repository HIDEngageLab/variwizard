/**
 * \file varikey_device.hpp
 * \author Koch, Roman (koch.roman@gmail.com)
 *
 * Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#ifndef __VARIKEY_DEVICE_HPP__
#define __VARIKEY_DEVICE_HPP__

#include <stdint.h>

#include "varikey_gadget.hpp"

#define VARIKEY_SERIAL_NUMBER_SIZE 12
#define VARIKEY_NAME_SIZE 32

namespace varikey
{
    struct device
    {
        uint32_t bustype;
        uint16_t vendor;
        uint16_t product;
        char name[VARIKEY_NAME_SIZE];
        uint8_t serial[VARIKEY_SERIAL_NUMBER_SIZE];
        uint32_t unique;
        gadget::type gadget;
        uint32_t hardware;
        uint32_t version;
    };
}

#endif /* __VARIKEY_DEVICE_HPP__ */
