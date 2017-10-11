#pragma once

#include <decl.h>
#include <stdint.h>

BEGIN_DECL

void usb_fs_device_init();

void usb_send_report(uint8_t *buffer, uint16_t nb);

END_DECL
