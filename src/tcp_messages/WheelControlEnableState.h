#include "ros_serialization.h"
#include <string>

namespace ax
{
enum WheelControlEnableState : int32_t
{
    UNKNOWN = 0,
    ENABLED = 1,
    DISABLED = 2
};

} // namespace ax

namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::WheelControlEnableState> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::WheelControlEnableState>
{
    template <typename Stream>
    inline static void write(Stream& stream, const ax::WheelControlEnableState v)
    {
        int32_t b = static_cast<int32_t>(v);
        memcpy(stream.advance(4), &b, 4);
    }

    template <typename Stream>
    inline static void read(Stream& stream, ax::WheelControlEnableState& v)
    {
        int32_t b;
        memcpy(&b, stream.advance(4), 4);
        v = (ax::WheelControlEnableState)(b);
    }

    inline static uint32_t serializedLength(ax::WheelControlEnableState) { return 4; }
};

} // namespace serialization
} // namespace ros
