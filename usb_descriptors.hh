#pragma once

/**
  * Prologue, or why all this madness
  * =================================
  *
  * USB is a complex protocol, possibly unnecessarily so. I've seen a lot
  * of binary-based protocols, and many were better than USB. Not all
  * though, and there are still a lot worse than that.
  *
  * But still. On paper, USB is a clean data tree, each node being a
  * descriptor. See http://www.beyondlogic.org/usbnutshell/usb5.shtml for
  * some details. When translated into binary however, things aren't that
  * straightforward anymore. The USB host will query the Device Descriptor
  * node alone first, but then will expect each whole Configuration
  * Descriptor in a single bulk. Or almost whole. When you get into
  * specific devices, such as the HID class, portions of the tree will
  * get sent out in separate parts, using a completely different format
  * than other descriptors, while collapsing neighbour nodes into a single
  * binary blob.
  *
  * In all fairness, each individual component still makes a lot of sense,
  * but there is little overall coherence, as if the USB specs were written
  * by a collection of individual workgroups who eventually compromised
  * into a patchworked document.
  *
  * Because of this, it's difficult to come up with a good way to write
  * the binary blobs describing a USB device.
  *
  * Most of the time, people will just copy/paste the same binary blobs
  * over and over, patching it to make it work for them, but with
  * little understanding overall of what's going on. There are also tools
  * that will spew out these binary blobs based on some way to describe
  * your device. Either these tools are going to be incomplete, not free,
  * platform-specific, or clunky. There are also people who try to do the
  * right thing, and will write code that handles the whole generation of
  * the descriptors at runtime. While this approach is probably the best
  * for consistency and sanity, its ROM and RAM footprint aren't exactly
  * adequate for low-end microcontroller usage.
  *
  * We are proposing an alternative way that provides a little bit of
  * the best of all worlds. The following code will allow you to
  * generate your descriptors at compilation time, with zero runtime cost,
  * either CPU or RAM, and with no extra ROM data, while describing them
  * using a typechecked tree structure. The drawback is it requires a
  * C++11 compiler, with a little bit of its headers. It doesn't require
  * any C++ runtime however, only purely header-based templates.
  *
  *
  * Rationale
  * =========
  *
  * We are going to use C++ templates and types to handle the logic of
  * the tree, and dealing with consistency and sanity checks. The
  * types themselves are sometimes going to hold member data, that will
  * eventually transform into the actual descriptor information we need
  * once we instanciate the root tree type. Therefore, the act of
  * instanciating the type will both trigger compilation-time checks,
  * and generate the data blob we will need to send over to the USB host.
  * The code will take care of all of the derived values, such as lenghts,
  * indexes, constants, or any other computed value the specification wants.
  *
  * But it's important to realize that the types are what are holding the
  * important information, not their members. Therefore, all of the code
  * is going to use a lot of C++ typing theory.
  *
  * C++11's constexpr is used throughout the code in order to make sure
  * we have absolutely zero runtime consequences.
  */

#include <stdint.h>
#include <cstddef>
#include <type_traits>
/**
  * In order to handle strings properly, we will need them as C++ types.
  * This header provides the typestring_is<> variadic template, that will
  * transform its string argument into a list of characters.
  *
  * This follows the rest of philosophy of the code where everything is
  * a type, including strings.
  */

#include "typestring.hh"

/**
  * This code will be gcc / clang only anyway, so I'm not going to do
  * anything special for Visual Studio and structure packing.
  */
#ifndef USB_PACKED
#define USB_PACKED __attribute__((packed))
#endif

namespace usb_template_helpers {

/**
  * This template transforms a uint16_t into a packed little endian
  * blob. Only uint8_t should be used overall.
  */
template<uint16_t value>
struct pack16 {
    uint8_t m_lo = value & 0xff;
    uint8_t m_hi = (value >> 8) & 0xff;
} USB_PACKED;

/**
  * We will make heavy usage of tuple<>-like structures all over the code,
  * but we can't use the std version, because we need better ordering control,
  * as well as better type checking. We also need specific things such as being
  * able to compute the offset of each element when it's being instanciated,
  * or the index of each element passed as an argument to the constructor.
  *
  * The way tuples work is fairly well known at that point, so I am not going
  * to explain this code. There are some fairly good references on the Internet
  * describing how tuples work internally, by creating a sequence in order to
  * be able to create unique types.
  */
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

/**
  * This concat<> helper will call | recursively on a tuple.
  */
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

} // namespace usb_template_helpers

namespace USB {

/**
  * Declaring StringDescriptor. The proper top container for a StringDescriptor
  * is a StringCollection. Strings are not part of any descriptor directly,
  * only their indexes within the collection. Therefore, the index returned
  * by StringCollection::find is what should be used as the value into
  * descriptors. See the example to see how this is used.
  */
template<size_t L>
struct StringDescriptorHeader {
    uint8_t m_bLenght = 2 + L * 2;
    uint8_t m_bDescriptorType = 3;
} USB_PACKED;

// I'm not going to try doing true UTF-8 or Unicode support,
// so let's limit ourselves to plain ASCII.
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

// Strings are stored in little endian unicode.
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

// String index 0 is special. While it is a normal StringDescriptor, its
// contents isn't a unicode string. Instead, it's a list of uint16 values
// that represents supported codepages. When requesting a StringDescriptor,
// the host will send the language ID it wants back. So technically we could
// create multiple StringCollections with different languages, and then
// selecting the proper one upon request. However, this would be a bit too
// much work for very little actual benefit. So we're only going to hardcode
// English (langid 0x409) as the only item in our list.
struct StringDescriptor0 : StringDescriptorBase {
    uint8_t m_bLength = 4;
    uint8_t m_bDescriptorType = 3;
    usb_template_helpers::pack16<0x409> m_wLANGID0;
} USB_PACKED;

// We basically inject our StringDescriptor0 at the beginning of our tuple,
// so that the indexes are properly aligned.
template<typename... strings>
using string_tuple = usb_template_helpers::typed_tuple<StringDescriptorBase, StringDescriptor0, strings...>;

struct StringCollectionBase { } USB_PACKED;
template<typename... strings>
struct StringCollection
    : StringCollectionBase
    , string_tuple<strings...> {
    using type = string_tuple<strings...>;

    typename type::offsets stringOffsets;
    // StringCollection being the top level type for StringDescriptors,
    // this is where we're going to return a pointer to a host request.
    constexpr const uint8_t * GetStringDescriptor(size_t index) const {
        return
            (index >= sizeof...(strings)) ? NULL :
            (index < 0) ? NULL :
            reinterpret_cast<const uint8_t *>(this) + reinterpret_cast<const ptrdiff_t *>(&stringOffsets)[index];
    }
} USB_PACKED;

constexpr int EmptyString = 0;


/**
  * Declaring EndpointDescriptor. We will automatically compute the Endpoint's
  * address based on its location inside the EndpointDescriptorList tuple.
  * So the only argument to EndpointAddress is going to be the direction in
  * which the Endpoint is operating.
  */
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


/**
  * Declaring the base type for the optional descriptors, such as HID's.
  */
struct OptionalDescriptorBase { } USB_PACKED;
struct OptionalDescriptorListBase { } USB_PACKED;
template<typename... types>
struct OptionalDescriptorList
    : OptionalDescriptorListBase
    , usb_template_helpers::typed_tuple<OptionalDescriptorBase, types...> { } USB_PACKED;


/**
  * Declaring InterfaceDescriptor and InterfaceDescriptorExtended. The only
  * difference between the two is the support for the OptionalDescriptorList,
  * useful for USB devices that require extra descriptors such as HID devices.
  */
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


/**
  * Declaring ConfigurationDescriptor.
  */
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
    usb_template_helpers::pack16<9 + sizeof(InterfaceDescriptorList)> m_wTotalLength;
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


/**
  * Declaring DeviceDescriptor. This is our second top level class that
  * requires instanciation in order to hold any data into the ROM.
  * Once instanciated, you can static_cast<uint8_t *> its pointer in order
  * to respond to a Device Descriptor request from the USB host.
  */
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
    // This method will return a pointer to a Configuration Descriptor, to
    // reply to a USB host request.
    constexpr const uint8_t * GetConfigurationDescriptor(size_t index) const {
        return
            (index > ConfigurationDescriptorList::bNumConfigurations) ? NULL :
            (index <= 0) ? NULL :
            reinterpret_cast<const uint8_t *>(&m_configurationDescriptors) + reinterpret_cast<const ptrdiff_t *>(&configurationOffsets)[index - 1];
    }
} USB_PACKED;

} // namespace USB

#undef USB_PACKED
