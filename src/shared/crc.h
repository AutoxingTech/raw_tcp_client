#pragma once
#include <stdint.h>

/// todo: improve performance:
/// https://github.com/LacobusVentura/MODBUS-CRC16
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