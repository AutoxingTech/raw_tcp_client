#include "../ros/ros_serialization.h"
#include <string>

namespace ax
{
class Header
{
public:
    uint32_t seq;
    ros::Time stamp;
    std::string frame_id;
};
} // namespace ax

//////////////////////////////////////////////////////////////////////////////
namespace ros
{
namespace serialization
{
template <>
struct Serializer<ax::Header>
{
    template <typename Stream, typename T>
    inline static void allInOne(Stream& stream, T m)
    {
        stream.next(m.seq);
        stream.next(m.stamp);
        stream.next(m.frame_id);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
};

} // namespace serialization
}; // namespace ros
