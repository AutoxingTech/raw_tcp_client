#include <iostream>
#include <cmath>
#include <unistd.h>
#include <memory>

#include "ros/message_wrapper.h"

#include "port_msgs/CustomMsgArray.h"
#include "port_msgs/WheelState.h"
#include "port_msgs/Odom.h"
#include "port_msgs/TcpRobotControl.h"
#include "port_msgs/TcpRobotState.h"
#include "port_msgs/DeviceState.h"
#include "packet/tcp_stream.h"

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
    to_buffer(msg, buffer);

    print_buffer(&buffer[0], buffer.size());

    CustomMsgArray msg2;
    if (from_buffer(msg2, &buffer[0], buffer.size()))
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
    to_buffer(msg, buffer);
    print_buffer(&buffer[0], buffer.size());

    WheelState msg2;
    if (from_buffer(msg2, &buffer[0], buffer.size()))
    {
        std::cout << msg2.enable_state << std::endl;
        std::cout << msg2.wheel_error_msg << std::endl;
    }
}

void test_odom()
{
    Odom msg;
    msg.stamp = ros::Time(1701769169.12340);
    msg.twist_linear_x = 1.123;
    msg.twist_linear_y = -2.345;
    msg.twist_angular = 3.14;

    std::vector<char> buffer;
    to_buffer(msg, buffer);
    print_buffer(&buffer[0], buffer.size());

    Odom msg2;
    if (from_buffer(msg2, &buffer[0], buffer.size()))
    {
        std::cout << msg2.stamp.sec << " " << msg2.stamp.nsec << std::endl;
        std::cout << msg2.twist_linear_x << std::endl;
        std::cout << msg2.twist_linear_y << std::endl;
        std::cout << msg2.twist_angular << std::endl;
    }
}

void test_wheel_enable()
{
    TcpRobotControl msg;
    msg.enable_wheels = true;

    std::vector<char> buffer;
    to_buffer(msg, buffer);
    print_buffer(&buffer[0], buffer.size());
}

void test_robot_state()
{
    TcpRobotState msg;
    msg.wheels_enabled = true;
    msg.battery_percent = 20;
    msg.is_charge = false;

    std::vector<char> buffer;
    to_buffer(msg, buffer);
    print_buffer(&buffer[0], buffer.size());
}

void test_device_state()
{
    DeviceState msg;
    msg.left_code = 1;
    msg.left_current = 2;
    msg.left_voltage = 3;
    msg.left_temperature = 4;
    msg.right_code = 5;
    msg.right_current = 6;
    msg.right_voltage = 7;
    msg.right_temperature = 8;

    std::vector<char> buffer;
    to_buffer(msg, buffer);
    print_buffer(&buffer[0], buffer.size());
}

void test_send()
{
    std::string m_hostIP = "127.0.0.1";
    int m_hostPort = 8091;
    std::shared_ptr<TcpStream> m_comStream = std::make_shared<TcpStream>();
    while (true)
    {
        if (!m_comStream->open(m_hostIP, m_hostPort))
        {
            sleep(1);
        }
        else
        {
            break;
        }
    }

    // send odom to server for test
    Odom msg;
    std::vector<char> buffer;
    for (int i = 0; i < INT32_MAX; i++)
    {
        msg.stamp = ros::Time::now();
        msg.twist_linear_x = i % 8 * 0.1;
        msg.twist_linear_y = i % 8 * -0.1;
        msg.twist_angular = i % 3;

        to_buffer(msg, buffer);
        // send
        int sizeOut = m_comStream->write((uint8_t*)(&buffer[0]), buffer.size());
        if (sizeOut <= 0)
        {
            printf("write length is %d\n", sizeOut);
        }

        buffer.clear();
        sleep(1);
    }
}

void test_recv()
{
    std::string m_hostIP = "127.0.0.1";
    int m_hostPort = 8091;
    std::shared_ptr<TcpStream> m_comStream = std::make_shared<TcpStream>();
    while (true)
    {
        if (!m_comStream->open(m_hostIP, m_hostPort))
        {
            sleep(1);
        }
        else
        {
            break;
        }
    }

    // recv odom from server for test
    Odom msg;
    char buffer[4096];
    for (int i = 0; i < INT32_MAX; i++)
    {
        buffer[0] = 0;
        int sizeOut = m_comStream->read((uint8_t*)buffer, 4096);
        if (sizeOut <= 0)
        {
            printf("write length is %d\n", sizeOut);
        }

        if (from_buffer(msg, buffer, 4096))
        {
            printf("recv twist_linear_x: %lf, twist_linear_y: %lf, twist_angular: %lf\n", msg.twist_linear_x,
                   msg.twist_linear_y, msg.twist_angular);
        }
        else
        {
            printf("from_buffer failed...");
        }

        sleep(1);
    }
}

void test_endian()
{
    // Big    Endian: 01 23 45 67
    // Little Endian: 67 45 23 01
    int value = 0x01234567;
    print_buffer(&value, sizeof(int));
}

int main()
{
    // test_custom_msg();
    // test_odom();
    // test_wheel_state();
    // test_wheel_enable();
    // test_device_state();
    // test_robot_state();

    test_recv();

    return 0;
}
