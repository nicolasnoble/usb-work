export BOARD = stm32f4discovery
ROOTDIR = $(CURDIR)/uC-sdk

default: all

TARGET = test-usb.bin
TARGET_SRCS = ./main.c \
  ./st-example-usb-descriptors.cc \
  ./Libraries/STM32_USB_Device_Library/Class/hid/src/usbd_hid_core.c \
  ./Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
  ./Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
  ./Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
  ./Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
  ./Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c \
  ./Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \

LIBDEPS = \
$(ROOTDIR)/FreeRTOS/libFreeRTOS.a \
$(ROOTDIR)/arch/libarch.a \
$(ROOTDIR)/os/libos.a \
$(ROOTDIR)/libc/libc.a \
$(ROOTDIR)/libm/libm.a \
$(ROOTDIR)/acorn/libacorn.a \
$(ROOTDIR)/hardware/libhardware.a \

LIBS = -Wl,--start-group $(LIBDEPS) -Wl,--end-group
TARGET_INCLUDES = \
  ./include \
  ./Libraries/STM32_USB_Device_Library/Class/hid/inc \
  ./Libraries/STM32_USB_Device_Library/Core/inc \
  ./Libraries/STM32_USB_OTG_Driver/inc \

TARGET_CFLAGS = \
  -DUSE_STDPERIPH_DRIVER \
  -DSTM32F4XX \
  -DUSE_USB_OTG_FS \

TARGET_CXXFLAGS = -std=c++11

include $(ROOTDIR)/common.mk

all: uC-sdk $(TARGET)

clean: clean-generic
	$(Q)$(MAKE) $(MAKE_OPTS) -C $(ROOTDIR) clean

.PHONY: uC-sdk

$(ROOTDIR)/FreeRTOS/libFreeRTOS.a: uC-sdk
$(ROOTDIR)/arch/libarch.a: uC-sdk
$(ROOTDIR)/os/libos.a: uC-sdk
$(ROOTDIR)/libc/libc.a: uC-sdk
$(ROOTDIR)/libm/libm.a: uC-sdk
$(ROOTDIR)/acorn/libacorn.a: uC-sdk
$(ROOTDIR)/hardware/libhardware.a: uC-sdk

uC-sdk:
	$(E) "[MAKE]   Entering uC-sdk"
	$(Q)$(MAKE) $(MAKE_OPTS) -C $(ROOTDIR)

deps: ldeps
	$(E) "[DEPS]   Creating dependency tree for uC-sdk"
	$(Q)$(MAKE) $(MAKE_OPTS) -C $(ROOTDIR) ldeps

include $(ROOTDIR)/FreeRTOS/config.mk
include $(ROOTDIR)/arch/config.mk
include $(ROOTDIR)/os/config.mk
include $(ROOTDIR)/libc/config.mk
include $(ROOTDIR)/libm/config.mk
include $(ROOTDIR)/acorn/config.mk
include $(ROOTDIR)/hardware/config.mk
include $(ROOTDIR)/target-rules.mk

