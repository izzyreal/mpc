#pragma once

#include <vector>

namespace mpc::file::all
{
    class Header
    {
    public:
        std::vector<char> loadBytes;
        std::vector<char> saveBytes;

        std::vector<char> &getHeaderArray();
        bool verifyFileID() const;

        std::vector<char> &getBytes();

        Header(const std::vector<char> &loadBytes);
        Header();
    };
} // namespace mpc::file::all
