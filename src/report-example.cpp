/**
 * \file report-example.cpp
 * \author Koch, Roman (koch.roman@googlemail.com)
 * 
 * Copyright (c) 2024, Roman Koch, koch.roman@gmail.com
 * SPDX-License-Identifier: MIT
 */

#include <fcntl.h>
#include <linux/hidraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/hiddev.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <linux/usbdevice_fs.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define HID_REPORT_ID 0x01 // Report ID für das HID-Gerät
#define REPORT_SIZE 15     // Größe des Reports

int main()
{
    int device_handle;
    const char *device_path = "/dev/hidraw4"; // Pfad zum HID-Gerät

    // Öffne das HID-Gerät
    device_handle = open(device_path, O_RDWR);
    if (device_handle < 0)
    {
        perror("Fehler beim Öffnen des HID-Geräts");
        return 1;
    }

    int result;

/*
    char bla[15] = {0};
    struct usbdevfs_ctrltransfer ctrl_transfer = {
        .bRequestType = 0xa1,
        .bRequest = 0x01,
        .wValue = 0x03a0,
        .wIndex = 0,
        .wLength = 15,
        .data = (void *)&bla[0],
    };
*/

    // Senden des Feature-Get-Requests
    //result = ioctl(device_handle, USBDEVFS_CONTROL, &ctrl_transfer);
    /*
    if (result < 0)
    {
        perror("Fehler beim Senden des Feature-Get-Requests");
        close(device_handle);
        return 1;
    }
    */
    // Daten, die im Feature-Report gesendet werden sollen
    unsigned char feature_report[REPORT_SIZE] = {
        0x51,
        0x02,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
    };

    // Setze Report ID und Daten
    //feature_report[0] = HID_REPORT_ID;

    // Senden des Feature-Reports über ioctl
    result = ioctl(device_handle, HIDIOCGFEATURE(REPORT_SIZE), feature_report);
    if (result < 0)
    {
        perror("Fehler beim Senden des Feature-Reports");
        close(device_handle);
        return 1;
    }

    printf("Feature-Report erfolgreich gesendet\n");

    // Schließe das Gerät
    close(device_handle);

    return 0;
}
