#include <cstdint>
#include <limits>

namespace ros
{
/**
 * \brief Base class for all exceptions thrown by ROS
 */
class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what) : std::runtime_error(what) {}
};

class Time
{
public:
    uint32_t sec;
    uint32_t nsec;

    Time() : sec(0), nsec(0) {}
    Time(uint32_t _sec, uint32_t _nsec) : sec(_sec), nsec(_nsec) {}
    explicit Time(double t) { fromSec(t); }

    Time& fromSec(double t)
    {
        int64_t sec64 = static_cast<int64_t>(floor(t));
        if (sec64 < 0 || sec64 > std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Time is out of dual 32-bit range");
        sec = static_cast<uint32_t>(sec64);
        nsec = static_cast<uint32_t>(std::round((t - sec) * 1e9));
        // avoid rounding errors
        sec += (nsec / 1000000000ul);
        nsec %= 1000000000ul;
        return *this;
    }
};

class Duration
{
public:
    uint32_t sec;
    uint32_t nsec;
};
} // namespace ros
