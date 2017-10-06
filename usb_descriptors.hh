#pragma once

#include <stdint.h>
#include <cstddef>
#include <type_traits>
#include "typestring.hh"

#define USB_PACKED __attribute__((packed))

namespace usb_template_helpers {

template<uint16_t value>
struct pack16 {
    uint8_t m_lo = value & 0xff;
    uint8_t m_hi = (value >> 8) & 0xff;
} USB_PACKED;

template<size_t... indices>
struct index_sequence {
    using type = index_sequence<indices...>;
} USB_PACKED;
template<size_t index, typename sequence>
struct cat_index_sequence;
template<size_t index, size_t... indices>
struct cat_index_sequence<index, index_sequence<indices...>> : index_sequence<indices..., index> { } USB_PACKED;
template<size_t N>
struct make_index_sequence : cat_index_sequence<N - 1, typename make_index_sequence<N - 1>::type>::type { } USB_PACKED;
template<>
struct make_index_sequence<1> : index_sequence<0> { } USB_PACKED;

template<size_t index, typename basetype, typename type>
struct tuple_notindexed_element {
    constexpr tuple_notindexed_element() {
        static_assert(std::is_base_of<basetype, type>::value, "Invalid tuple element");
    }
    type m_value;
} USB_PACKED;

template<size_t index, typename basetype, typename type>
struct tuple_indexed_element {
    constexpr tuple_indexed_element() : m_value(index) {
        static_assert(std::is_base_of<basetype, type>::value, "Invalid tuple indexed element");
    }
    type m_value;
} USB_PACKED;

template<ptrdiff_t offset, typename head, typename... tail>
struct offset_calculator {
    ptrdiff_t m_offset = offset;
    offset_calculator<offset + sizeof(head), tail...> m_next_offsets;
} USB_PACKED;

template<ptrdiff_t offset, typename tail>
struct offset_calculator<offset, tail> {
    ptrdiff_t m_offset = offset;
} USB_PACKED;

template<typename>
constexpr int count() { return 0; }
template<typename type, typename head, typename... tail>
constexpr int count() {
    return (std::is_same<type, head>::value ? 1 : 0) + count<type, tail...>();
}

template<typename>
constexpr int find_inner(int) { return -1; }
template<typename type, typename head, typename... tail>
constexpr int find_inner(int index = 0) {
    return std::is_same<type, head>::value ? index : find_inner<type, tail...>(index + 1);
}

template<size_t index, bool indexed, typename basetype, typename type>
struct tuple_element :
    std::conditional<indexed,
        tuple_indexed_element<index, basetype, type>,
        tuple_notindexed_element<index, basetype, type>
    >::type { } USB_PACKED;
template<typename sequences, bool indexed, typename basetype, typename... types>
struct tuple_impl;
template<size_t... indices, bool indexed, typename basetype, typename... types>
struct tuple_impl<index_sequence<indices...>, indexed, basetype, types...> :
    tuple_element<indices, indexed, basetype, types>... {
    using offsets = offset_calculator<0, types...>;
    template<typename type>
    static constexpr int find() {
        static_assert(count<type, types...>() <= 1, "get_index() failed: too many duplicate entries");
        static_assert(count<type, types...>() == 1, "get_index() failed: couldn't find entry");
        return find_inner<type, types...>();
    }
} USB_PACKED;

template<typename basetype, bool indexed, typename... types>
struct typed_tuple_generic : tuple_impl<typename make_index_sequence<sizeof...(types)>::type, indexed, basetype, types...> { } USB_PACKED;
template<typename basetype, typename... types>
struct typed_tuple : typed_tuple_generic<basetype, false, types...> { } USB_PACKED;
template<typename basetype, typename... types>
struct typed_indexed_tuple : typed_tuple_generic<basetype, true, types...> { } USB_PACKED;

template<size_t index, typename head, typename... tail>
struct type_at_index {
    using type = typename type_at_index<index - 1, tail...>::type;
} USB_PACKED;

template<typename head, typename... tail>
struct type_at_index<0, head, tail...> {
    using type = head;
} USB_PACKED;

template<size_t outer_index, size_t inner_index, typename basetype, typename type>
struct inner_tuple_element {
    constexpr inner_tuple_element() : m_value(outer_index, inner_index) {
        static_assert(std::is_base_of<basetype, type>::value, "Wrong embedded tuple type");
    }
    type m_value;
} USB_PACKED;

template<size_t outer_index, typename basetype, typename sequences, typename... types>
struct inner_tuple_impl;
template<size_t outer_index, typename basetype, size_t... indices, typename... types>
struct inner_tuple_impl<outer_index, basetype, index_sequence<indices...>, types...> :
    inner_tuple_element<outer_index, indices, basetype, types>... { } USB_PACKED;

template<typename... types>
struct inner_tuple {
    using type = inner_tuple<types...>;
} USB_PACKED;

template<size_t index, typename basetype, typename type>
struct embedded_tuple_element_unpacked;
template<size_t index, typename basetype, typename... types>
struct embedded_tuple_element_unpacked<index, basetype, inner_tuple<types...>> :
    inner_tuple_impl<index, basetype, typename make_index_sequence<sizeof...(types)>::type, types...> { } USB_PACKED;

template<size_t index, typename basetype, typename type>
struct embedded_tuple_element : embedded_tuple_element_unpacked<index, basetype, typename type::type> { } USB_PACKED;

template<typename basetype, typename sequences, typename... types>
struct embedded_tuple_impl;

template<typename basetype, size_t... indices, typename... types>
struct embedded_tuple_impl<basetype, index_sequence<indices...>, types...> :
    embedded_tuple_element<indices, basetype, types>... { } USB_PACKED;

template<typename basetype, typename... types>
struct embedded_tuple :
    embedded_tuple_impl<basetype, typename make_index_sequence<sizeof...(types)>::type, types...> { } USB_PACKED;

template<typename rettype, typename basetype, typename type>
constexpr rettype concat(rettype dummy, basetype ref, type value) {
    static_assert(std::is_same<basetype, type>::value, "Wrong type for concatenation");
    return static_cast<rettype>(value);
}
template<typename rettype, typename basetype, typename type, typename... Args>
constexpr rettype concat(rettype dummy, basetype ref, type first, Args... args) {
    static_assert(std::is_same<basetype, type>::value, "Wrong type for concatenation");
    return static_cast<rettype>(first) | concat(dummy, ref, args...);
}

}

namespace USB {

template<size_t L>
struct StringDescriptorHeader {
    uint8_t m_bLenght = 2 + L;
    uint8_t m_bDescriptorType = 3;
} USB_PACKED;

template<size_t index, char C>
struct CharChecker {
    constexpr CharChecker() {
        static_assert(C < 127, "Invalid (non ascii) character in a USB string");
        static_assert(C >= 32, "Invalid (non ascii) character in a USB string");
    }
} USB_PACKED;

template<typename indices, char... C>
struct StringCheckerInner;
template<size_t... indices, char... C>
struct StringCheckerInner<usb_template_helpers::index_sequence<indices...>, C...> : CharChecker<indices, C>... { } USB_PACKED;

template<char... C>
struct StringChecker : StringCheckerInner<typename usb_template_helpers::make_index_sequence<sizeof...(C)>::type, C...> { } USB_PACKED;

template<size_t index, char C>
struct StringChar : usb_template_helpers::pack16<C> { } USB_PACKED;

template<typename indices, char... C>
struct StringContentsInner;
template<size_t... indices, char... C>
struct StringContentsInner<usb_template_helpers::index_sequence<indices...>, C...> : StringChar<indices, C>... { } USB_PACKED;

template<char... C>
struct StringContents : StringContentsInner<typename usb_template_helpers::make_index_sequence<sizeof...(C)>::type, C...> { } USB_PACKED;

struct StringDescriptorBase { } USB_PACKED;
template<typename type>
struct StringDescriptor;
template<char... C>
struct StringDescriptor<irqus::typestring<C...>>
    : StringDescriptorBase
    , StringChecker<C...>
    , StringDescriptorHeader<sizeof...(C)>
    , StringContents<C...> { } USB_PACKED;

struct StringDescriptor0 : StringDescriptorBase {
    uint8_t m_bLength = 4;
    uint8_t m_bDescriptorType = 3;
    usb_template_helpers::pack16<0x409> m_wLANGID[1];
} USB_PACKED;

template<typename... strings>
using string_tuple = usb_template_helpers::typed_tuple<StringDescriptorBase, StringDescriptor0, strings...>;

struct StringCollectionBase { } USB_PACKED;
template<typename... strings>
struct StringCollection
    : StringCollectionBase
    , string_tuple<strings...> {
    using type = string_tuple<strings...>;

    typename type::offsets stringOffsets;
    constexpr const uint8_t * GetStringDescriptor(size_t index) const {
        return
            (index >= sizeof...(strings)) ? NULL :
            (index < 0) ? NULL :
            reinterpret_cast<const uint8_t *>(this) + reinterpret_cast<const ptrdiff_t *>(&stringOffsets)[index];
    }
} USB_PACKED;

constexpr int EmptyString = 0;

enum Direction {
    Out = 0,
    In = 128,
};
struct EndpointAddressBase { } USB_PACKED;
template<Direction direction>
struct EndpointAddress : EndpointAddressBase {
    constexpr EndpointAddress(size_t index) : m_value(direction | (index + 1)) { }
    uint8_t m_value;
} USB_PACKED;

struct EndpointAttributesBase { } USB_PACKED;
struct ControlEndpoint : EndpointAttributesBase {
    uint8_t m_value = 0;
} USB_PACKED;
struct BulkEndpoint : EndpointAttributesBase {
    uint8_t m_value = 2;
} USB_PACKED;
struct InterruptEndpoint : EndpointAttributesBase {
    uint8_t m_value = 3;
} USB_PACKED;
enum SynchronisationType {
    NoSynchronisation,
    Asynchrnous,
    Adaptive,
    Synchronous,
};
enum UsageType {
    DataEndpoint,
    FeedbackEndpoint,
    ExplicitFeedback,
};
template<
    SynchronisationType synchronisationType,
    UsageType usageType
>
struct IsochronousEndpoint : EndpointAttributesBase {
    uint8_t m_value =
        1 |
        (static_cast<uint8_t>(synchronisationType) << 2) |
        (static_cast<uint8_t>(usageType) << 4);
};

struct EndpointMaxPacketSizeBase { } USB_PACKED;
template<uint16_t value>
struct EndpointMaxPacketSize
    : EndpointMaxPacketSizeBase
    , usb_template_helpers::pack16<value> { } USB_PACKED;

struct IntervalBase { } USB_PACKED;
template<uint8_t value>
struct Interval : IntervalBase {
    uint8_t m_value = value;
} USB_PACKED;

struct EndpointDescriptorBase { } USB_PACKED;
template<
    typename EndpointAddress,
    typename EndpointAttributes,
    typename EndpointMaxPacketSize,
    typename Interval
>
struct EndpointDescriptor : EndpointDescriptorBase {
    constexpr EndpointDescriptor(size_t index) : m_bEndpointAddress(index) {
        static_assert(std::is_base_of<EndpointAddressBase, EndpointAddress>::value, "Wrong EndpointAddress type");
        static_assert(std::is_base_of<EndpointAttributesBase, EndpointAttributes>::value, "Wrong EndpointAttributes type");
        static_assert(std::is_base_of<EndpointMaxPacketSizeBase, EndpointMaxPacketSize>::value, "Wrong EndpointMaxPacketSize type");
        static_assert(std::is_base_of<IntervalBase, Interval>::value, "Wrong Interval type");
    }
    uint8_t m_bLength = 7;
    uint8_t m_bDescriptorType = 5;
    EndpointAddress m_bEndpointAddress;
    EndpointAttributes m_bmAttributes;
    EndpointMaxPacketSize m_wMaxPacketSize;
    Interval m_bInterval;
} USB_PACKED;

struct EndpointDescriptorListBase { } USB_PACKED;
template<typename... types>
struct EndpointDescriptorList
    : EndpointDescriptorListBase
    , usb_template_helpers::typed_indexed_tuple<EndpointDescriptorBase, types...> {
    static constexpr size_t bNumEndpoints = sizeof...(types);
} USB_PACKED;

struct OptionalDescriptorBase { } USB_PACKED;
struct OptionalDescriptorListBase { } USB_PACKED;
template<typename... types>
struct OptionalDescriptorList
    : OptionalDescriptorListBase
    , usb_template_helpers::typed_tuple<OptionalDescriptorBase, types...> { } USB_PACKED;

struct InterfaceClassBase { } USB_PACKED;
template<uint8_t value>
struct InterfaceClass : InterfaceClassBase {
    uint8_t m_value = value;
} USB_PACKED;
struct InterfaceClass_AUDIO : InterfaceClass<0x01> { } USB_PACKED;
struct InterfaceClass_COMM_CDCCTRL : InterfaceClass<0x02> { } USB_PACKED;
struct InterfaceClass_HID : InterfaceClass<0x03> { } USB_PACKED;
struct InterfaceClass_PHYSICAL : InterfaceClass<0x05> { } USB_PACKED;
struct InterfaceClass_IMAGE : InterfaceClass<0x06> { } USB_PACKED;
struct InterfaceClass_PRINTER : InterfaceClass<0x07> { } USB_PACKED;
struct InterfaceClass_STORAGE : InterfaceClass<0x08> { } USB_PACKED;
struct InterfaceClass_CDCDATA : InterfaceClass<0x0a> { } USB_PACKED;
struct InterfaceClass_SMARTCARD : InterfaceClass<0x0b> { } USB_PACKED;
struct InterfaceClass_CONTENTSECURITY : InterfaceClass<0x0d> { } USB_PACKED;
struct InterfaceClass_VIDEO : InterfaceClass<0x0e> { } USB_PACKED;
struct InterfaceClass_HEALTH : InterfaceClass<0x0f> { } USB_PACKED;
struct InterfaceClass_AUDIOVIDEO : InterfaceClass<0x10> { } USB_PACKED;
struct InterfaceClass_USBCBRIDGE : InterfaceClass<0x12> { } USB_PACKED;
struct InterfaceClass_DIAGNOSTIC : InterfaceClass<0xdc> { } USB_PACKED;
struct InterfaceClass_WIRELESS : InterfaceClass<0xe0> { } USB_PACKED;
struct InterfaceClass_MISC : InterfaceClass<0xef> { } USB_PACKED;
struct InterfaceClass_APPSPECIFIC : InterfaceClass<0xfe> { } USB_PACKED;
struct InterfaceClass_VENDORSPECIFIC : InterfaceClass<0xff> { } USB_PACKED;

struct InterfaceSubClassBase { } USB_PACKED;
template<uint8_t value>
struct InterfaceSubClass : InterfaceSubClassBase {
    uint8_t m_value = value;
} USB_PACKED;

struct InterfaceProtocolBase { } USB_PACKED;
template<uint8_t value>
struct InterfaceProtocol : InterfaceProtocolBase {
    uint8_t m_value = value;
} USB_PACKED;

struct InterfaceDescriptorBase { } USB_PACKED;
template<
    typename InterfaceClass,
    typename InterfaceSubClass,
    typename InterfaceProtocol,
    int InterfaceString,
    size_t NumEndpoints
>
struct InterfaceDescriptorInner : InterfaceDescriptorBase {
    constexpr InterfaceDescriptorInner(size_t outer_index, size_t inner_index) : m_bInterfaceNumber(outer_index), m_bAlternateSetting(inner_index) {
        static_assert(std::is_base_of<InterfaceClassBase, InterfaceClass>::value, "Wrong InterfaceClass type");
        static_assert(std::is_base_of<InterfaceSubClassBase, InterfaceSubClass>::value, "Wrong InterfaceSubClass type");
        static_assert(std::is_base_of<InterfaceProtocolBase, InterfaceProtocol>::value, "Wrong InterfaceProtocol type");
    }
    uint8_t m_bLength = 9;
    uint8_t m_bDescriptorType = 4;
    uint8_t m_bInterfaceNumber;
    uint8_t m_bAlternateSetting;
    uint8_t m_bNumEndpoints = NumEndpoints;
    InterfaceClass m_bInterfaceClass;
    InterfaceSubClass m_bInterfaceSubClass;
    InterfaceProtocol m_bInterfaceProtocol;
    uint8_t m_iInterface = InterfaceString;
} USB_PACKED;
template<
    typename InterfaceClass,
    typename InterfaceSubClass,
    typename InterfaceProtocol,
    int InterfaceString,
    typename EndpointDescriptorList
>
struct InterfaceDescriptor
    : InterfaceDescriptorInner<
        InterfaceClass,
        InterfaceSubClass,
        InterfaceProtocol,
        InterfaceString,
        EndpointDescriptorList::bNumEndpoints>
    , EndpointDescriptorList {
    constexpr InterfaceDescriptor(size_t outer_index, size_t inner_index)
        : InterfaceDescriptorInner<
            InterfaceClass,
            InterfaceSubClass,
            InterfaceProtocol,
            InterfaceString,
            EndpointDescriptorList::bNumEndpoints>(outer_index, inner_index) {
        static_assert(std::is_base_of<EndpointDescriptorListBase, EndpointDescriptorList>::value, "Wrong EndpointDescriptorList type");
    }
} USB_PACKED;
template<
    typename InterfaceClass,
    typename InterfaceSubClass,
    typename InterfaceProtocol,
    int InterfaceString,
    typename OptionalDescriptorList,
    typename EndpointDescriptorList
>
struct InterfaceDescriptorExtended
    : InterfaceDescriptorInner<
        InterfaceClass,
        InterfaceSubClass,
        InterfaceProtocol,
        InterfaceString,
        EndpointDescriptorList::bNumEndpoints>
    , OptionalDescriptorList
    , EndpointDescriptorList {
    constexpr InterfaceDescriptorExtended(size_t outer_index, size_t inner_index)
        : InterfaceDescriptorInner<
            InterfaceClass,
            InterfaceSubClass,
            InterfaceProtocol,
            InterfaceString,
            EndpointDescriptorList::bNumEndpoints>(outer_index, inner_index) {
        static_assert(std::is_base_of<OptionalDescriptorListBase, OptionalDescriptorList>::value, "Wrong OptionalDescriptorList type");
        static_assert(std::is_base_of<EndpointDescriptorListBase, EndpointDescriptorList>::value, "Wrong EndpointDescriptorList type");
    }
} USB_PACKED;

struct InterfaceAlternateListBase { } USB_PACKED;
template<typename... types>
struct InterfaceAlternateList
    : InterfaceAlternateListBase
    , usb_template_helpers::inner_tuple<types...> { } USB_PACKED;

struct InterfaceDescriptorListBase { } USB_PACKED;
template<typename... alternates>
struct InterfaceDescriptorList
    : InterfaceDescriptorListBase
    , usb_template_helpers::embedded_tuple<InterfaceDescriptorBase, alternates...> {
    static constexpr size_t bNumInterfaces = sizeof...(alternates);
} USB_PACKED;

enum ConfigurationAttributesEnum {
    ConfigurationRemoteWakeup = 0x20,
    ConfigurationSelfPowered = 0x40,
};
struct ConfigurationAttributesBase { } USB_PACKED;

struct MaxPowerBase { } USB_PACKED;
template<unsigned value>
struct MaxPower : MaxPowerBase {
    constexpr MaxPower() {
        static_assert(value < 512, "MaxPower value too high");
    }
    uint8_t m_value = (value + 1) >> 1;
} USB_PACKED;

template<ConfigurationAttributesEnum... values>
struct ConfigurationAttributes : ConfigurationAttributesBase {
    static constexpr uint8_t value = 0x80 | usb_template_helpers::concat(static_cast<uint8_t>(0), static_cast<ConfigurationAttributesEnum>(0), values...);
} USB_PACKED;
template<>
struct ConfigurationAttributes<> : ConfigurationAttributesBase {
    static constexpr uint8_t value = 0x80;
} USB_PACKED;

struct ConfigurationDescriptorBase { } USB_PACKED;
template <
    typename ConfigurationAttributes,
    typename MaxPower,
    int ConfigurationString,
    typename InterfaceDescriptorList
>
struct ConfigurationDescriptor : ConfigurationDescriptorBase {
    constexpr ConfigurationDescriptor(size_t index) : m_bConfigurationValue(index + 1) {
        static_assert(std::is_base_of<ConfigurationAttributesBase, ConfigurationAttributes>::value, "Wrong ConfigurationAttributes type");
        static_assert(std::is_base_of<MaxPowerBase, MaxPower>::value, "Wrong MaxPower type");
        static_assert(std::is_base_of<InterfaceDescriptorListBase, InterfaceDescriptorList>::value, "Wrong InterfaceDescriptorList type");
    }
    uint8_t m_bLength = 9;
    uint8_t m_bDescriptorType = 2;
    uint16_t m_wTotalLength = 9 + sizeof(InterfaceDescriptorList);
    uint8_t m_bNumInterfaces = InterfaceDescriptorList::bNumInterfaces;
    uint8_t m_bConfigurationValue;
    uint8_t m_iConfiguration = ConfigurationString;
    uint8_t m_bmAttributes = ConfigurationAttributes::value;
    MaxPower m_bMaxPower;
    InterfaceDescriptorList m_interfaceDescriptors;
} USB_PACKED;

struct ConfigurationDescriptorListBase { } USB_PACKED;
template<typename... types>
struct ConfigurationDescriptorList
    : ConfigurationDescriptorListBase
    , usb_template_helpers::typed_indexed_tuple<ConfigurationDescriptorBase, types...> {
    static constexpr size_t bNumConfigurations = sizeof...(types);
} USB_PACKED;

struct SpecificationNumberBase { } USB_PACKED;
template<uint16_t value>
struct SpecificationNumber : SpecificationNumberBase, usb_template_helpers::pack16<value> { } USB_PACKED;
struct USB1_0 : SpecificationNumber<0x100> { } USB_PACKED;
struct USB1_1 : SpecificationNumber<0x110> { } USB_PACKED;
struct USB2_0 : SpecificationNumber<0x200> { } USB_PACKED;

struct DeviceClassBase { } USB_PACKED;
template<uint8_t value>
struct DeviceClass : DeviceClassBase { uint8_t m_value = value; } USB_PACKED;
struct DeviceClass_NONE : DeviceClass<0x00> { } USB_PACKED;
struct DeviceClass_COMM_CDCCTRL : DeviceClass<0x02> { } USB_PACKED;
struct DeviceClass_HUB : DeviceClass<0x09> { } USB_PACKED;
struct DeviceClass_BILLBOARD : DeviceClass<0x11> { } USB_PACKED;
struct DeviceClass_DIAGNOSTIC : DeviceClass<0xdc> { } USB_PACKED;
struct DeviceClass_MISC : DeviceClass<0xef> { } USB_PACKED;
struct DeviceClass_VENDORSPECIFIC : DeviceClass<0xff> { } USB_PACKED;

struct DeviceSubClassBase { } USB_PACKED;
template<uint8_t value>
struct DeviceSubClass : DeviceSubClassBase { uint8_t m_value = value; } USB_PACKED;

struct DeviceProtocolBase { } USB_PACKED;
template<uint8_t value>
struct DeviceProtocol : DeviceProtocolBase { uint8_t m_value = value; } USB_PACKED;

struct MaxPacketSizeBase { } USB_PACKED;
template<uint8_t value>
struct MaxPacketSize : MaxPacketSizeBase {
    constexpr MaxPacketSize() {
        static_assert(value == 8 || value == 16 || value == 32 || value == 64, "Wrong MaxPacketSize - must be 8, 16, 32, or 64");
    }
    uint8_t m_value = value;
} USB_PACKED;

struct VendorIDBase { } USB_PACKED;
template<uint16_t value>
struct VendorID
    : VendorIDBase
    , usb_template_helpers::pack16<value> { } USB_PACKED;

struct ProductIDBase { } USB_PACKED;
template<uint16_t value>
struct ProductID
    : ProductIDBase
    , usb_template_helpers::pack16<value> { } USB_PACKED;

struct DeviceReleaseNumberBase { } USB_PACKED;
template<uint16_t value>
struct DeviceReleaseNumber : DeviceReleaseNumberBase, usb_template_helpers::pack16<value> { } USB_PACKED;

template <
    typename SpecificationNumber,
    typename DeviceClass,
    typename DeviceSubClass,
    typename DeviceProtocol,
    typename MaxPacketSize,
    typename VendorID,
    typename ProductID,
    typename DeviceReleaseNumber,
    int ManufacturerString,
    int ProductString,
    int SerialNumberString,
    typename ConfigurationDescriptorList
>
struct DeviceDescriptor {
    constexpr DeviceDescriptor() {
        static_assert(std::is_base_of<SpecificationNumberBase, SpecificationNumber>::value, "Wrong SpecificationNumber type");
        static_assert(std::is_base_of<DeviceClassBase, DeviceClass>::value, "Wrong DeviceClass type");
        static_assert(std::is_base_of<DeviceSubClassBase, DeviceSubClass>::value, "Wrong DeviceSubClass type");
        static_assert(std::is_base_of<DeviceProtocolBase, DeviceProtocol>::value, "Wrong DeviceProtocol type");
        static_assert(std::is_base_of<MaxPacketSizeBase, MaxPacketSize>::value, "Wrong MaxPacketSize type");
        static_assert(std::is_base_of<VendorIDBase, VendorID>::value, "Wrong VendorID type");
        static_assert(std::is_base_of<ProductIDBase, ProductID>::value, "Wrong ProductID type");
        static_assert(std::is_base_of<DeviceReleaseNumberBase, DeviceReleaseNumber>::value, "Wrong DeviceReleaseNumber type");
        static_assert(std::is_base_of<ConfigurationDescriptorListBase, ConfigurationDescriptorList>::value, "Wrong ConfigurationDescriptorList type");
    }
    uint8_t m_bLength = 18;
    uint8_t m_bDescriptorType = 1;
    SpecificationNumber m_bcdUSB;
    DeviceClass m_bDeviceClass;
    DeviceSubClass m_bDeviceSubClass;
    DeviceProtocol m_bDeviceProtocol;
    MaxPacketSize m_bMaxPacketSize;
    VendorID m_idVendor;
    ProductID m_idProduct;
    DeviceReleaseNumber m_bcdDevice;
    uint8_t m_iManufacturer = ManufacturerString;
    uint8_t m_iProduct = ProductString;
    uint8_t m_iSerialNumber = SerialNumberString;
    uint8_t m_bNumConfigurations = ConfigurationDescriptorList::bNumConfigurations;
    ConfigurationDescriptorList m_configurationDescriptors;

    typename ConfigurationDescriptorList::offsets configurationOffsets;
    constexpr const uint8_t * GetConfigurationDescriptor(size_t index) const {
        return
            (index > ConfigurationDescriptorList::bNumConfigurations) ? NULL :
            (index <= 0) ? NULL :
            reinterpret_cast<const uint8_t *>(&m_configurationDescriptors) + reinterpret_cast<const ptrdiff_t *>(&configurationOffsets)[index - 1];
    }
} USB_PACKED;

}

#undef USB_PACKED