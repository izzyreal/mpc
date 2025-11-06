#include "BitUtil.hpp"

#include <bitset>
#include <stdexcept>

using namespace mpc::file;
using namespace std;

unsigned char BitUtil::removeUnusedBits(unsigned char b,
                                        const vector<int> &usedRange)
{
    auto cleanedByte = b;
    auto range_to_clear = invertRange(usedRange);
    for (int pos = range_to_clear[0]; pos < range_to_clear[1]; pos++)
    {
        cleanedByte &= ~(1 << pos);
    }

    return cleanedByte;
}

vector<int> BitUtil::invertRange(const vector<int> &range)
{
    auto invRange = vector<int>(2);
    if (range[0] < range[1])
    {
        invRange[0] = range[1] + 1;
        invRange[1] = 8 - 1;
    }
    else
    {
        invRange[0] = 0;
        invRange[1] = range[0] - 1;
    }
    return invRange;
}

unsigned char BitUtil::setBit(unsigned char b, int i, bool on)
{
    auto bs = std::bitset<8>(b);
    bs.set(i, on);
    return static_cast<unsigned char>(bs.to_ulong());
}

bool BitUtil::isBitOn(unsigned char b, int i)
{
    if ((b & (1UL << i)) != 0)
    {
        return true;
    }

    return false;
}

unsigned char BitUtil::stitchBytes(unsigned char b1,
                                   const vector<int> &usedBits1,
                                   unsigned char b2,
                                   const vector<int> &usedBits2)
{
    auto byte1occupiesstart = usedBits1[0] == 0;
    if (byte1occupiesstart)
    {
        if (usedBits2[0] <= usedBits1[1])
        {
            throw std::invalid_argument("stitch error");
        }

        if (usedBits2[0] - 1 != usedBits1[1])
        {
            throw std::invalid_argument("stitch error");
        }
    }
    else
    {
        if (usedBits1[0] <= usedBits2[1])
        {
            throw std::invalid_argument("stitch error");
        }

        if (usedBits1[0] - 1 != usedBits2[1])
        {
            throw std::invalid_argument("stitch error");
        }
    }
    unsigned char result = 0;

    for (auto i = usedBits1[0]; i <= usedBits1[1]; i++)
    {
        auto on = isBitOn(b1, i);
        result = setBit(result, i, on);
    }

    for (auto i = usedBits2[0]; i <= usedBits2[1]; i++)
    {
        auto on = isBitOn(b2, i);
        result = setBit(result, i, on);
    }
    return result;
}

string BitUtil::getBits(int8_t b)
{
    return bitset<8>(b).to_string();
}

char BitUtil::setBits(int8_t mask, char byte, bool newValue)
{
    auto res = byte;
    if (!newValue)
    {
        res &= ~mask;
    }
    else
    {
        res |= mask;
    }
    return res;
}
