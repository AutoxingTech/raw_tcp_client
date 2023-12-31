#include "packet/tcp_pack.h"
#include "../shared/crc.h"
#include <cstdio>

ParserResult MsgPackParser::feed(const uint8_t* bytes, size_t n, size_t* bytesUsed)
{
    // header(2) + length(4) + crc(2) + body
    if (n < sizeof(MsgPack))
        return ParserResult_incomplete;

    uint32_t payloadLength = *(uint32_t*)(bytes + sizeof(MsgPack::header));
    size_t completeLength = payloadLength + sizeof(MsgPack);
    if (n < completeLength)
        return ParserResult_incomplete;

    *bytesUsed = completeLength;

    uint16_t referCrc = *(uint16_t*)(bytes + sizeof(MsgPack::header) + sizeof(MsgPack::length));
    uint16_t calcCrc = calculateCRC16(bytes + sizeof(MsgPack), payloadLength);
    if (referCrc != calcCrc)
    {
        printf("check sum failed\n");
        return ParserResult_failed;
    }
    return ParserResult_succ;
}
