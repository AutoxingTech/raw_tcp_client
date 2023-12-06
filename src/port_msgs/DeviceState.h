#pragma once

#include "../ros/ros_serialization.h"

namespace ax
{
class DeviceState
{
public:
    constexpr static char magic_header[2] = {'D', 'S'};

    uint16_t left_voltage;
    uint16_t left_current;
    uint16_t left_temperature;
    uint16_t left_code;

    uint16_t right_voltage;
    uint16_t right_current;
    uint16_t right_temperature;
    uint16_t right_code;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::DeviceState> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::DeviceState>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.left_voltage);
        stream.next(m.left_current);
        stream.next(m.left_temperature);
        stream.next(m.left_code);
        stream.next(m.right_voltage);
        stream.next(m.right_current);
        stream.next(m.right_temperature);
        stream.next(m.right_code);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
