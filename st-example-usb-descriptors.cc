//descriptors
#include "usb_descriptors.hh"

typedef USB::StringDescriptor<typestring_is("GrumpyCoders")> manufacturer;
typedef USB::StringDescriptor<typestring_is("Custom HID device")> product;
typedef USB::StringDescriptor<typestring_is("00000000011C")> serial;
typedef USB::StringDescriptor<typestring_is("HID Config")> config;
typedef USB::StringDescriptor<typestring_is("CDC Config")> config2;
typedef USB::StringDescriptor<typestring_is("HID Interface")> interface;
typedef USB::StringDescriptor<typestring_is("CDC Interface Ctrl")> interface2;
typedef USB::StringDescriptor<typestring_is("CDC Interface Data")> interface3;

typedef USB::StringCollection<
    manufacturer,
    product,
    serial,
    config,
    config2,
    interface,
    interface2,
    interface3
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
        >,
        USB::ConfigurationDescriptor<
            USB::ConfigurationAttributes<
                USB::ConfigurationSelfPowered
            >,
            USB::MaxPower<50>,
            strings::find<config2>(), //0 in st descriptor
            USB::InterfaceDescriptorList<
                USB::InterfaceAlternateList<
                    USB::InterfaceDescriptorExtended<
                        USB::InterfaceClass_CDC,
                        USB::InterfaceSubClass<2>,
                        USB::InterfaceProtocol<1>,
                        strings::find<interface2>(), //0 in st descriptor
                        USB::OptionalDescriptorList<
                            USB::CDC::FunctionalDescriptor<
                                USB::CDC::FunctionalDescriptorType_CS_Interface,
                                USB::CDC::FunctionalDescriptorSubType_Header,
                                USB::CDC::FunctionSpecificDataList<
                                    USB::CDC::FunctionSpecificData<0x10>, /* bcdCDC: spec release number */
                                    USB::CDC::FunctionSpecificData<0x01>
                                >
                            >,
                            USB::CDC::FunctionalDescriptor<
                                USB::CDC::FunctionalDescriptorType_CS_Interface,
                                USB::CDC::FunctionalDescriptorSubType_Call,
                                USB::CDC::FunctionSpecificDataList<
                                    USB::CDC::FunctionSpecificData<0x00>, /* bmCapabilities: D0+D1 */
                                    USB::CDC::FunctionSpecificData<0x01>  /* bDataInterface: 1 */
                                >
                            >,
                            USB::CDC::FunctionalDescriptor<
                                USB::CDC::FunctionalDescriptorType_CS_Interface,
                                USB::CDC::FunctionalDescriptorSubType_ACM,
                                USB::CDC::FunctionSpecificDataList<
                                    USB::CDC::FunctionSpecificData<0x02> /* bmCapabilities */
                                >
                            >,
                            USB::CDC::FunctionalDescriptor<
                                USB::CDC::FunctionalDescriptorType_CS_Interface,
                                USB::CDC::FunctionalDescriptorSubType_Union,
                                USB::CDC::FunctionSpecificDataList<
                                    USB::CDC::FunctionSpecificData<0x00>, /* bMasterInterface: Communication class interface */
                                    USB::CDC::FunctionSpecificData<0x01>  /* bSlaveInterface0: Data Class Interface */
                                >
                            >
                        >,
                        USB::EndpointDescriptorList<
                            USB::EndpointDescriptor<
                                USB::EndpointAddress<USB::In>, //0x82
                                USB::InterruptEndpoint,
                                USB::EndpointMaxPacketSize<8>,
                                USB::Interval<0xff>
                            >
                        >
                    >,
                    USB::InterfaceDescriptor<
                        USB::InterfaceClass_CDCDATA,
                        USB::InterfaceSubClass<0>,
                        USB::InterfaceProtocol<0>,
                        strings::find<interface3>(), //0 in st descriptor
                        USB::EndpointDescriptorList<
                            USB::EndpointDescriptor<
                                USB::EndpointAddress<USB::Out>, //0x01
                                USB::InterruptEndpoint,
                                USB::EndpointMaxPacketSize<64>,
                                USB::Interval<0>
                            >,
                            USB::EndpointDescriptor<
                                USB::EndpointAddress<USB::In>, //0x81
                                USB::InterruptEndpoint,
                                USB::EndpointMaxPacketSize<64>,
                                USB::Interval<0>
                            >
                        >
                    >
                >
            >
        >
    >
> device_descriptor;


extern "C" const uint8_t * get_USB_first_interface_descriptor(int configuration) {
    return device_descriptor.GetFirstInterfaceDescriptor(configuration);
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
