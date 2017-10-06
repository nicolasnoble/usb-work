//descriptors
#include "usb_descriptors.hh"

typedef USB::StringDescriptor<typestring_is("???")> lang;
typedef USB::StringDescriptor<typestring_is("GrumpyCoders")> manufacturer;
typedef USB::StringDescriptor<typestring_is("Custom HID device")> product;
typedef USB::StringDescriptor<typestring_is("00000000011C")> serial;
typedef USB::StringDescriptor<typestring_is("HID Config")> config;
typedef USB::StringDescriptor<typestring_is("HID Interface")> interface;

typedef USB::StringCollection<
    lang,
    manufacturer,
    product,
    serial,
    config,
    interface
> strings;

static const strings strings_collection;

struct ExtraDescriptor : USB::OptionalDescriptorBase {
    uint8_t m_extra[9] = {
        0x09,
        0x21,
        0x11,
        0x01,
        0x00,
        0x01,
        0x22,
        0x4a,
        0x00
    };
} __attribute__((packed));

static const USB::DeviceDescriptor<
    USB::USB2_0,
    USB::DeviceClass_NONE,
    USB::DeviceSubClass<0>,
    USB::DeviceProtocol<0>,
    USB::MaxPacketSize<64>,
    USB::VendorID<0x483>,
    USB::ProductID<0x5710>,
    USB::DeviceReleaseNumber<0x200>,
    strings::find<manufacturer>(),
    strings::find<product>(),
    strings::find<serial>(),
    USB::ConfigurationDescriptorList<
        USB::ConfigurationDescriptor<
            USB::ConfigurationAttributes<
                USB::ConfigurationRemoteWakeup,
                USB::ConfigurationSelfPowered
            >,
            USB::MaxPower<100>,
            strings::find<config>(),
            USB::InterfaceDescriptorList<
                USB::InterfaceAlternateList<
                    USB::InterfaceDescriptorExtended<
                        USB::InterfaceClass_HID,
                        USB::InterfaceSubClass<1>,
                        USB::InterfaceProtocol<2>,
                        strings::find<interface>(),
                        USB::OptionalDescriptorList<ExtraDescriptor>,
                        USB::EndpointDescriptorList<
                            USB::EndpointDescriptor<
                                USB::EndpointAddress<USB::In>,
                                USB::InterruptEndpoint,
                                USB::EndpointMaxPacketSize<4>,
                                USB::Interval<10>
                            >
                        >
                    >
                >
            >
        >
    >
> device_descriptor;

extern "C" const uint8_t * get_USB_configuration_descriptor(int index) {
    return device_descriptor.GetConfigurationDescriptor(index);
}

extern "C" const uint8_t * get_USB_device_descriptor() {
    return reinterpret_cast<const uint8_t *>(&device_descriptor);
}

extern "C" const uint8_t * get_USB_string_descriptor(int index) {
    return strings_collection.GetStringDescriptor(index);
}


/*
uint8_t *  USBD_USR_ConfigStrDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_DeviceDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_LangIDStrDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_ProductStrDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_ManufacturerStrDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_SerialStrDescriptor( uint8_t speed , uint16_t *length)
uint8_t *  USBD_USR_InterfaceStrDescriptor( uint8_t speed , uint16_t *length)
*/