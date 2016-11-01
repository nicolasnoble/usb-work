#pragma once

//Descriptors
#define USB_DESC_DEVICE                 0x01
#define USB_DESC_CONFIG                 0x02
#define USB_DESC_STRING                 0x03
#define USB_DESC_INTERFACE              0x04
#define USB_DESC_ENDPOINT               0x05
#define USB_DESC_BOS                    0x0f
#define USB_DESC_DEVICE_CAPABILITY      0x10
#define USB_DESC_HID                    0x21
#define USB_DESC_REPORT                 0x22
#define USB_DESC_PHYSICAL               0x23
#define USB_DESC_HUB                    0x29
#define USB_DESC_SUPERSPEED_HUB         0x2a
#define USB_DESC_SS_ENDPOINT_COMPANION  0x30

//Classes
#define USB_CLASS_FROM_INTERFACE        0x00
#define USB_CLASS_AUDIO                 0x01
#define USB_CLASS_COMM                  0x02
#define USB_CLASS_HID                   0x03
#define USB_CLASS_PHYSICAL              0x05
#define USB_CLASS_PRINTER               0x07
#define USB_CLASS_IMAGE                 0x06
#define USB_CLASS_MASS_STORAGE          0x08
#define USB_CLASS_HUB                   0x09
#define USB_CLASS_DATA                  0x0a
#define USB_CLASS_SMART_CARD            0x0b
#define USB_CLASS_CONTENT_SECURITY      0x0d
#define USB_CLASS_VIDEO                 0x0e
#define USB_CLASS_PERSONAL_HEALTHCARE   0x0f
#define USB_CLASS_DIAGNOSTIC_DEVICE     0xdc
#define USB_CLASS_WIRELESS              0xe0
#define USB_CLASS_APPLICATION           0xfe
#define USB_CLASS_VENDOR_SPEC           0xff
