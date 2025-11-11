#include "ByteUtil.hpp"
#include <sstream>

using namespace mpc::file;

std::uint16_t ByteUtil::bytes2ushort(const std::vector<char> &ca)
{
    auto hi = static_cast<std::uint16_t>(static_cast<unsigned char>(ca[1]));
    auto lo = static_cast<std::uint16_t>(static_cast<unsigned char>(ca[0]));
    return hi << 8 | lo;
}

std::int16_t ByteUtil::bytes2short(const std::vector<char> &ca)
{
    auto hi = static_cast<std::uint16_t>(static_cast<unsigned char>(ca[1]));
    auto lo = static_cast<std::uint16_t>(static_cast<unsigned char>(ca[0]));
    return static_cast<std::int16_t>(hi << 8 | lo);
}

std::uint32_t ByteUtil::bytes2uint(const std::vector<char> &quartet)
{
    std::uint32_t result = 0;
    result |= static_cast<std::uint32_t>(static_cast<unsigned char>(quartet[3]))
              << 24;
    result |= static_cast<std::uint32_t>(static_cast<unsigned char>(quartet[2]))
              << 16;
    result |= static_cast<std::uint32_t>(static_cast<unsigned char>(quartet[1]))
              << 8;
    result |=
        static_cast<std::uint32_t>(static_cast<unsigned char>(quartet[0]));
    return result;
}

std::vector<char> ByteUtil::uint2bytes(std::uint32_t n)
{
    auto res = std::vector<char>(4);
    res[3] = static_cast<char>(n >> 24 & 0xFF);
    res[2] = static_cast<char>(n >> 16 & 0xFF);
    res[1] = static_cast<char>(n >> 8 & 0xFF);
    res[0] = static_cast<char>(n & 0xFF);
    return res;
}

std::vector<char> ByteUtil::ushort2bytes(std::uint16_t n)
{
    auto res = std::vector<char>(2);
    res[1] = static_cast<char>(n >> 8 & 0xFF);
    res[0] = static_cast<char>(n & 0xFF);
    return res;
}

std::vector<char> ByteUtil::short2bytes(std::int16_t n)
{
    auto res = std::vector<char>(2);
    res[1] = static_cast<char>(n >> 8 & 0xFF);
    res[0] = static_cast<char>(n & 0xFF);
    return res;
}

std::vector<char>
ByteUtil::stitchByteArrays(const std::vector<std::vector<char>> &byteArrays)
{
    unsigned int totalSize = 0;
    for (const auto &ba : byteArrays)
    {
        totalSize += static_cast<unsigned int>(ba.size());
    }
    auto result = std::vector<char>(totalSize);
    int counter = 0;
    for (const auto &ba : byteArrays)
    {
        for (auto b : ba)
        {
            result[counter++] = b;
        }
    }
    return result;
}

char ByteUtil::hexToByte(const std::string &charPair)
{
    unsigned short byte = 0;
    std::istringstream iss(charPair.c_str());
    iss >> std::hex >> byte;
    return static_cast<char>(byte % 0x100);
}

std::vector<char> ByteUtil::hexToBytes(const std::string &charPairs)
{
    std::vector<char> res;
    for (size_t i = 0; i < charPairs.size(); i += 2)
    {
        std::string current;
        current.push_back(charPairs[i]);
        current.push_back(charPairs[i + 1]);
        res.push_back(hexToByte(current));
    }
    return res;
}

std::vector<std::int16_t> ByteUtil::bytesToShorts(const std::vector<char> &src)
{
    if (src.empty() || src.size() % 2 != 0)
    {
        return std::vector<std::int16_t>();
    }

    int length = static_cast<int>(src.size()) / 2;
    auto res = std::vector<std::int16_t>(length);
    int charCounter = 0;
    std::vector<char> pair(2);

    for (int i = 0; i < length; i++)
    {
        pair[0] = src[charCounter++];
        pair[1] = src[charCounter++];
        res[i] = bytes2short(pair);
    }

    return res;
}
