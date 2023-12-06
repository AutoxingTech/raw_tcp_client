#pragma once

#include "../ros/ros_serialization.h"
#include <string>

namespace ax
{
class CustomMsg
{
public:
    CustomMsg() = default;

    CustomMsg(const std::string& name, float linear_velocity_x, float linear_velocity_y, float angular_velocity)
        : name(name), linear_velocity_x(linear_velocity_x), linear_velocity_y(linear_velocity_y),
          angular_velocity(angular_velocity)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const CustomMsg& obj)
    {
        os << obj.name << " " << obj.linear_velocity_x << " " << obj.linear_velocity_y << " " << obj.angular_velocity;
        return os;
    }

    std::string name;
    float linear_velocity_x = 0;
    float linear_velocity_y = 0;
    float angular_velocity = 0;
};

class CustomMsgArray
{
public:
    constexpr static char magic_header[2] = {'B', '2'};

    std::array<CustomMsg, 2> msgs;
    std::vector<CustomMsg> msgs_vector;
};

} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::CustomMsg> : public FalseType
{
};
} // namespace message_traits
namespace serialization
{
template <>
struct Serializer<ax::CustomMsg>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.name);
        stream.next(m.linear_velocity_x);
        stream.next(m.linear_velocity_y);
        stream.next(m.angular_velocity);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

template <>
struct Serializer<ax::CustomMsgArray>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.msgs);
        stream.next(m.msgs_vector);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
