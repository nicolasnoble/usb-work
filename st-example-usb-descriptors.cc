//descriptors
#include "usb_descriptors.hh"

typedef USB::StringDescriptor<typestring_is("GrumpyCoders")> manufacturer;
typedef USB::StringDescriptor<typestring_is("Custom HID device")> product;
typedef USB::StringDescriptor<typestring_is("00000000011C")> serial;
typedef USB::StringDescriptor<typestring_is("HID Config")> config;
typedef USB::StringDescriptor<typestring_is("HID Interface")> interface;

typedef USB::StringCollection<
    manufacturer,
    product,
    serial,
    config,
    interface
> strings;

static const strings strings_collection;

//For reminder purpose, not used anymore
struct ExtraDescriptor : USB::OptionalDescriptorBase {
    uint8_t m_extra[9] = {
        0x09, // b len
        0x21, // b descriptor type (0x21 = HID)
        0x11, // w HID version
        0x01, // w 1.11
        0x00, // b country
        0x01, // b number of hid class
        0x22, // b report descriptor type
        0x4a, // w total length of
        0x00  // w report descriptor
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
            strings::find<config>(), //0 in st descriptor
            USB::InterfaceDescriptorList<
                USB::InterfaceAlternateList<
                    USB::InterfaceDescriptorExtended<
                        USB::InterfaceClass_HID,
                        USB::InterfaceSubClass<1>,
                        USB::InterfaceProtocol<2>,
                        strings::find<interface>(), //0 in st descriptor
                        USB::OptionalDescriptorList<
                            USB::HID::HIDDescriptor<
                                USB::HID::CountryCode_Not_Supported,
                                USB::HID::ReportDescriptorIndexList<
                                    USB::HID::ReportDescriptorIndex<0x004a>
                                >
                            >
                        >,
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

extern "C" const uint8_t * get_USB_interface_descriptor(int configuration, int interface) {
    return device_descriptor.GetConfigurationDescriptor(configuration).GetInterfaceDescriptor(interface);
}

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
