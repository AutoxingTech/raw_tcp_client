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
void to_buffer(const MessageType& msg, std::vector<char>& buffer)
{
    // buffer: old data + (WrapperHeader + new msg data)
    size_t header_size = sizeof(WrapperHeader);
    uint32_t msg_length = ros::serialization::serializationLength(msg);
    size_t old_size = buffer.size();
    buffer.resize(old_size + sizeof(WrapperHeader) + msg_length);

    WrapperHeader wrapper_header;
    wrapper_header.magic[0] = MessageType::magic_header[0];
    wrapper_header.magic[1] = MessageType::magic_header[1];
    wrapper_header.data_length = msg_length;

    ros::serialization::OStream stream((uint8_t*)&buffer[old_size + header_size], msg_length);
    ros::serialization::serialize(stream, msg);

    wrapper_header.crc16 = calculateCRC16(&buffer[old_size + header_size], msg_length);
    memcpy(&buffer[old_size], &wrapper_header, sizeof(wrapper_header));
}

template <typename MessageType>
bool from_buffer(MessageType& msg, const char* buffer, size_t buffer_size)
{
    if (buffer[0] != MessageType::magic_header[0] || buffer[1] != MessageType::magic_header[1])
    {
        return false;
    }

    if (buffer_size < sizeof(WrapperHeader))
    {
        return false;
    }

    WrapperHeader* wrapper_header = (WrapperHeader*)buffer;
    if (buffer_size < sizeof(WrapperHeader) + wrapper_header->data_length)
    {
        return false;
    }

    if (sizeof(WrapperHeader) != 8)
    {
        return false;
    }

    uint16_t crc16 = calculateCRC16(buffer + sizeof(WrapperHeader), wrapper_header->data_length);
    if (crc16 != wrapper_header->crc16)
    {
        return false;
    }

    ros::serialization::IStream istream((uint8_t*)(buffer + sizeof(WrapperHeader)), wrapper_header->data_length);
    ros::serialization::deserialize(istream, msg);

    return true;
}
} // namespace ax
