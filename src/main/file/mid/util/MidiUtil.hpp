#pragma once

#include <vector>
#include <string>

namespace mpc::file::mid::util
{
    class MidiUtil
    {
    public:
        static int bytesToInt(const std::vector<char> &buff, int off, int len);
        static std::vector<char> intToBytes(int val, int byteCount);
        static bool bytesEqual(const std::vector<char> &buf1,
                               const std::vector<char> &buf2, int off, int len);

    private:
        static std::string HEX;

    public:
        static std::string byteToHex(char b);
        static std::string bytesToHex(const std::vector<char> &b);
    };
} // namespace mpc::file::mid::util
