#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace mpc::file
{
    class ByteUtil
    {
    public:
        static std::uint32_t bytes2uint(const std::vector<char> &quartet);
        static std::int16_t bytes2short(const std::vector<char> &pair);
        static std::uint16_t bytes2ushort(const std::vector<char> &pair);
        static std::vector<char> uint2bytes(std::uint32_t n);
        static std::vector<char> ushort2bytes(std::uint16_t n);
        static std::vector<char> short2bytes(std::int16_t n);

        static std::vector<short> bytesToShorts(const std::vector<char> &src);

        static std::vector<char>
        stitchByteArrays(const std::vector<std::vector<char>> &byteArrays);

        static char hexToByte(const std::string &charPair);
        static std::vector<char> hexToBytes(const std::string &charPairs);
    };
} // namespace mpc::file
