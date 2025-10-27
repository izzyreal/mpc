#pragma once

#include <string>
#include <vector>

namespace mpc::file
{

    class BitUtil
    {

    public:
        static unsigned char removeUnusedBits(unsigned char b,
                                              std::vector<int> usedRange);
        static std::vector<int> invertRange(std::vector<int> range);
        static unsigned char setBit(unsigned char b, int i, bool on);
        static bool isBitOn(unsigned char b, int i);
        static unsigned char stitchBytes(unsigned char b1,
                                         std::vector<int> usedBits1,
                                         unsigned char b2,
                                         std::vector<int> usedBits2);
        static std::string getBits(int8_t b);
        static char setBits(int8_t mask, char byte, bool newValue);
    };

} // namespace mpc::file