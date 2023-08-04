#include "ByteUtil.hpp"

#include <sstream>

using namespace mpc::file;

std::uint16_t ByteUtil::bytes2ushort(std::vector<char> ca) {
    unsigned char pair[2];
    pair[0] = ca[0];
    pair[1] = ca[1];
    return (pair[1] << 8) | pair[0];
}

std::int16_t ByteUtil::bytes2short(std::vector<char> ca) {
    return (ca[1] << 8) | (ca[0] & 0xff);
}

std::uint32_t ByteUtil::bytes2uint(std::vector<char> quartet) {
    char ca[4];
    for (int i = 0; i < 4; i++)
        ca[i] = quartet[i];
    return  *(std::uint32_t *)ca;
}

std::vector<char> ByteUtil::uint2bytes(std::uint32_t n) {
    auto res = std::vector<char>(4);
    res[3] = (n >> 24) & 0xFF;
    res[2] = (n >> 16) & 0xFF;
    res[1] = (n >> 8) & 0xFF;
    res[0] = n & 0xFF;
    return res;
}

std::vector<char> ByteUtil::ushort2bytes(std::uint16_t n) {
    auto res = std::vector<char>(2);
    res[1] = (n >> 8);
    res[0] = n & 0xFF;
    return res;
}

std::vector<char> ByteUtil::short2bytes(std::int16_t n) {
    auto res = std::vector<char>(2);
    res[1] = (n >> 8) & 0xFF;
    res[0] = n & 0xff;
    return res;
}

std::vector<char> ByteUtil::stitchByteArrays(const std::vector<std::vector<char>>& byteArrays)
{
    unsigned int totalSize = 0;
    for (auto& ba : byteArrays) {
        totalSize += (int) ba.size();
    }
    auto result = std::vector<char>(totalSize);
    int counter = 0;
    for (auto& ba : byteArrays) {
        for (auto b : ba)
            result[counter++] = b;
    }
    return result;
}


char ByteUtil::hexToByte(const std::string& charPair)
{
    unsigned short byte = 0;
    std::istringstream iss(charPair.c_str());
    iss >> std::hex >> byte;
    return byte % 0x100;
}

std::vector<char> ByteUtil::hexToBytes(std::string charPairs)
{
    std::vector<char> res;
    for (size_t i = 0; i < charPairs.size(); i += 2) {
        std::string current;
        current.push_back(charPairs[i]);
        current.push_back(charPairs[i + 1]);
        res.push_back(hexToByte(current));
    }
    return res;
}

std::vector<std::int16_t> ByteUtil::bytesToShorts(std::vector<char> src)
{
    if (src.empty() || src.size() % 2 != 0)
    {
        return std::vector<std::int16_t>(0);
    }

    int length = (int) (src.size()) / 2;
    auto res = std::vector<std::int16_t>(length);
    int charCounter = 0;
    auto pair = std::vector<char>(2);

    for (int i = 0; i < length; i++)
    {
        pair[0] = src[charCounter++];
        pair[1] = src[charCounter++];
        res[i] = bytes2short(pair);
    }

    return res;
}
