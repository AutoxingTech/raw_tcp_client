#include <iostream>
#include <cmath>

#include "ros/message_wrapper.h"

#include "port_msgs/CustomMsgArray.h"
#include "port_msgs/WheelState.h"

using namespace ax;

inline void print_buffer(const void* buffer, size_t length)
{
    const uint8_t* p = (const uint8_t*)buffer;
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x ", p[i]);
    }
    printf("\n");
}

void test_custom_msg()
{
    CustomMsgArray msg;
    msg.msgs[0] = CustomMsg("aaaa", 0.1, 0.2, 5 * M_PI / 180);
    msg.msgs[1] = CustomMsg("bbbbbbbb", 0.1, 0.2, 5 * M_PI / 180);

    std::vector<char> buffer;
    to_buffer(msg, "AB", buffer);

    print_buffer(&buffer[0], buffer.size());

    CustomMsgArray msg2;
    if (from_buffer(msg2, "AB", &buffer[0], buffer.size()))
    {
        std::cout << msg2.msgs[0] << std::endl;
        std::cout << msg2.msgs[1] << std::endl;
    }
}

void test_wheel_state()
{
    WheelState msg;
    msg.enable_state = WheelControlEnableState::ENABLED;
    msg.wheel_error_msg = "abc";

    std::vector<char> buffer;
    to_buffer(msg, "AB", buffer);
    print_buffer(&buffer[0], buffer.size());

    WheelState msg2;
    if (from_buffer(msg2, "AB", &buffer[0], buffer.size()))
    {
        std::cout << msg2.enable_state << std::endl;
        std::cout << msg2.wheel_error_msg << std::endl;
    }
}

int main()
{
    test_custom_msg();
    test_wheel_state();

    return 0;
}
