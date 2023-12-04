#include "../ros/ros_serialization.h"
#include <string>

namespace ax
{
class Odom
{
public:
    constexpr static char magic_header[2] = {'A', 'B'};

    ros::Time stamp;
    float twist_linear_x;
    float twist_linear_y;
    float twist_angular;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::Odom> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::Odom>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.stamp);
        stream.next(m.twist_linear_x);
        stream.next(m.twist_linear_y);
        stream.next(m.twist_angular);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
