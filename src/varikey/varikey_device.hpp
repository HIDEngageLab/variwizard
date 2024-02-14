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

namespace varikey
{
    static const size_t SERIAL_NUMBER_SIZE = 12;
    static const size_t PLATFORM_SIZE = 32;
    static const size_t PRODUCT_SIZE = 32;
    static const size_t NAME_SIZE = 32;

    struct device
    {
        uint32_t bustype;
        uint16_t vendor;
        uint16_t product;
        char name[NAME_SIZE];
        uint8_t serial[SERIAL_NUMBER_SIZE];
        uint32_t unique;
        uint16_t firmware;
        uint16_t revision;
        uint16_t patch;
        uint16_t build;
        uint16_t maintainer;
        uint16_t hardware;
        uint8_t number;
        uint8_t variant;
    };
}

#endif /* __VARIKEY_DEVICE_HPP__ */
