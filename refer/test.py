import struct


def print_bytes(bs):
    res = ""
    for b in bs:
        res += "0x%02x, " % b
    print(res)


# The hex value given
value = 0xC016147B

# Pack the hex value into bytes as an unsigned 32-bit integer in little-endian format
packed = struct.pack("<I", value)
print_bytes(packed)  # 0x7b, 0x14, 0x16, 0xc0,

# Now unpack those bytes as a float
f3 = struct.unpack("<f", packed)[0]
print(f3)
