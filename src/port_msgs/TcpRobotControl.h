#pragma once

#include "../ros/ros_serialization.h"

namespace ax
{
class TcpRobotControl
{
public:
    constexpr static char magic_header[2]{(char)0xba, (char)0xe1};
    bool enable_wheels;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::TcpRobotControl> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::TcpRobotControl>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.enable_wheels);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
