#pragma once

#include "../ros/ros_serialization.h"

namespace ax
{
class WheelEnable
{
public:
    constexpr static char magic_header[2]{(char)0xba, (char)0xe1};
    bool enable_state;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::WheelEnable> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::WheelEnable>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.enable_state);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
