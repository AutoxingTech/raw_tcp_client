#pragma once

#include "../ros/ros_serialization.h"

namespace ax
{
class TcpRobotState
{
public:
    constexpr static char magic_header[2]{(char)0xab, (char)0xd0};
    bool wheels_enabled;
    uint8_t battery_percent;
    bool is_charge;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::TcpRobotState> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::TcpRobotState>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.wheels_enabled);
        stream.next(m.battery_percent);
        stream.next(m.is_charge);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
