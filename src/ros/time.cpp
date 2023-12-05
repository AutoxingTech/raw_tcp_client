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

#include "time.h"

namespace ros
{
/*********************************************************************
 ** Cross Platform Functions
 *********************************************************************/
void ros_walltime(uint32_t& sec, uint32_t& nsec)
{
#if !defined(_WIN32)
#    if HAS_CLOCK_GETTIME
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    if (start.tv_sec < 0 || start.tv_sec > std::numeric_limits<uint32_t>::max())
        throw std::runtime_error("Timespec is out of dual 32-bit range");
    sec = start.tv_sec;
    nsec = start.tv_nsec;
#    else
    struct timeval timeofday;
    gettimeofday(&timeofday, NULL);
    if (timeofday.tv_sec < 0 || timeofday.tv_sec > std::numeric_limits<uint32_t>::max())
        throw std::runtime_error("Timeofday is out of dual signed 32-bit range");
    sec = timeofday.tv_sec;
    nsec = timeofday.tv_usec * 1000;
#    endif
#else
    uint64_t now_s = 0;
    uint64_t now_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();

    normalizeSecNSec(now_s, now_ns);

    sec = (uint32_t)now_s;
    nsec = (uint32_t)now_ns;
#endif
}

} // namespace ros
