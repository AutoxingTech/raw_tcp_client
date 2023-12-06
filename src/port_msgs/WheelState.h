#pragma once

#include "../ros/ros_serialization.h"
#include "WheelControlEnableState.h"
#include <string>

namespace ax
{
class WheelState
{
public:
    constexpr static char magic_header[2] = {'A', '3'};

    WheelControlEnableState enable_state;
    std::string wheel_error_msg;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::WheelState> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::WheelState>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.enable_state);
        stream.next(m.wheel_error_msg);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
