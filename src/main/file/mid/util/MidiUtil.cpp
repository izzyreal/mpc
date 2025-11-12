#include "file/mid/util/MidiUtil.hpp"

using namespace mpc::file::mid::util;

int MidiUtil::bytesToInt(const std::vector<char> &buff, int off, int len)
{
    int num = 0;
    int shift = 0;
    for (int i = off + len - 1; i >= off; i--)
    {
        num += (buff[i] & 255) << shift;
        shift += 8;
    }
    return num;
}

std::vector<char> MidiUtil::intToBytes(int val, int byteCount)
{

    auto buffer = std::vector<char>(byteCount);
    auto ints = std::vector<char>(byteCount);
    for (int i = 0; i < byteCount; i++)
    {
        ints[i] = val & 255;
        buffer[byteCount - i - 1] = ints[i];
        val = val >> 8;
        if (val == 0)
        {
            break;
        }
    }
    return buffer;
}

bool MidiUtil::bytesEqual(const std::vector<char> &buf1,
                          const std::vector<char> &buf2, int off, int len)
{
    for (int i = off; i < off + len; i++)
    {
        if (i >= buf1.size() || i >= buf2.size())
        {
            return false;
        }
        if (buf1[i] != buf2[i])
        {
            return false;
        }
    }
    return true;
}

std::string MidiUtil::HEX = "0123456789ABCDEF";

std::string MidiUtil::byteToHex(char b)
{

    int high = (b & 240) >> 4;
    int low = b & 15;
    std::string res;
    res.push_back(HEX[high]);
    res.push_back(HEX[low]);
    return res;
}

std::string MidiUtil::bytesToHex(const std::vector<char> &b)
{
    std::string res = "";
    for (int i = 0; i < b.size(); i++)
    {
        res += byteToHex(b[i]) + " ";
    }
    return res;
}
