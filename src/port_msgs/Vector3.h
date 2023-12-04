#include "../ros/ros_serialization.h"
#include <string>

namespace ax
{
class Vector3
{
public:
    double x, y, z;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace message_traits
{
template <>
struct IsFixedSize<ax::Vector3> : public TrueType
{
};
} // namespace message_traits

namespace serialization
{
template <>
struct Serializer<ax::Vector3>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.x);
        stream.next(m.y);
        stream.next(m.z);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
