/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2010, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <cmath>
#include <stdexcept>
#include <sys/time.h>

namespace ros
{
#define HAS_CLOCK_GETTIME (_POSIX_C_SOURCE >= 199309L)

/*********************************************************************
 ** Cross Platform Functions
 *********************************************************************/
void ros_walltime(uint32_t& sec, uint32_t& nsec);

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

    static Time now()
    {
        Time t;
        ros_walltime(t.sec, t.nsec);
        return t;
    }
};

class Duration
{
public:
    uint32_t sec;
    uint32_t nsec;
};
} // namespace ros
