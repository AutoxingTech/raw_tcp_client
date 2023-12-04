#include <cstdint>
#include <vector>

#include "ros_serialization.h"

namespace ax
{
/// CRC-16 x16+x15+x2+1  <==> 0x8005
inline uint16_t calculateCRC16(const void* buffer, int len)
{
    const uint8_t* p = (const uint8_t*)buffer;
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

struct __attribute__((packed)) WrapperHeader
{
    char magic[2];
    uint32_t data_length;
    uint16_t crc16;
};

template <typename MessageType>
void to_buffer(const MessageType& msg, const char* header, std::vector<char>& buffer)
{
    uint32_t msg_length = ros::serialization::serializationLength(msg);
    size_t old_size = buffer.size();
    buffer.resize(old_size + 8 + msg_length);

    WrapperHeader wrapper_header;
    wrapper_header.magic[0] = header[0];
    wrapper_header.magic[1] = header[1];
    wrapper_header.data_length = msg_length;

    ros::serialization::OStream stream((uint8_t*)&buffer[old_size + 8], msg_length);
    ros::serialization::serialize(stream, msg);

    wrapper_header.crc16 = calculateCRC16(&buffer[old_size + 8], msg_length);
    memcpy(&buffer[old_size], &wrapper_header, sizeof(wrapper_header));
}

template <typename MessageType>
bool from_buffer(MessageType& msg, const char* header, const char* buffer, size_t buffer_size)
{
    if (buffer[0] != header[0] || buffer[1] != header[1])
    {
        printf("1\n");
        return false;
    }

    if (buffer_size < 8)
    {
        printf("2\n");
        return false;
    }

    WrapperHeader* wrapper_header = (WrapperHeader*)buffer;
    if (buffer_size < 8 + wrapper_header->data_length)
    {
        printf("3\n");
        return false;
    }

    if (sizeof(WrapperHeader) != 8)
    {
        printf("5\n");
        return false;
    }

    uint16_t crc16 = calculateCRC16(buffer + sizeof(WrapperHeader), wrapper_header->data_length);
    if (crc16 != wrapper_header->crc16)
    {
        printf("4\n");
        return false;
    }

    ros::serialization::IStream istream((uint8_t*)(buffer + sizeof(WrapperHeader)), wrapper_header->data_length);
    ros::serialization::deserialize(istream, msg);

    return true;
}
} // namespace ax
