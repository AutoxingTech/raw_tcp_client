#include <iostream>

using namespace std;

void printBuffer(const uint8_t* p, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x ", p[i]);
    }
    printf("\n");
}

/// CRC-16 x16+x15+x2+1  <==> 0x8005
uint16_t calculateCRC16(const uint8_t* p, int len)
{
    uint16_t crc = 0xffff;            /// init value
    const uint16_t CRC_MASK = 0xA001; /// high and low bit flipping of '0x8005'
    int i, j;
    for (j = 0; j < len; j++)
    {
        crc ^= p[j];
        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x0001) > 0)
            {
                crc = (crc >> 1) ^ CRC_MASK;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

struct __attribute__((packed)) WheelControlTwist
{
    WheelControlTwist(int16_t linear_velocity_x, int16_t linear_velocity_y, int16_t angular_velocity)
    {
        m_bufferLength = sizeof(m_buffer);
        size_t dataSize = m_bufferLength - sizeof(m_buffer.header) - sizeof(m_buffer.length) - sizeof(m_buffer.crc);

        m_buffer.length = (uint16_t)dataSize;
        m_buffer.linear_velocity_x = linear_velocity_x * 1000;
        m_buffer.linear_velocity_y = linear_velocity_y * 1000;
        m_buffer.angular_velocity = angular_velocity * 100;
        m_buffer.crc = calculateCRC16((const uint8_t*)(&m_buffer.linear_velocity_x), dataSize);
    }

    void* buffer() { return &m_buffer; }
    size_t bufferLength() const { return m_bufferLength; }

private:
    size_t m_bufferLength;
    static const int m_dataMaxSize = 10;

    // binary buffer
    struct __attribute__((packed))
    {
        const uint8_t header[4]{'W', 'T', 'S', 'T'};
        uint16_t length;
        uint16_t crc;
        int16_t linear_velocity_x;
        int16_t linear_velocity_y;
        int16_t angular_velocity;
    } m_buffer;
};

int main()
{
    // -1, 2, -3: 57 54 53 54 06 00 04 56 18 fc d0 07 d4 fe
    WheelControlTwist twist(-1, 2, -3);
    printBuffer((const uint8_t*)twist.buffer(), twist.bufferLength());

    return 0;
}
