#pragma once

#include <vector>
#include <string>

namespace mpc::file::aps
{

    class ApsName
    {

        static const int NAME_STRING_LENGTH{16};

    public:
        std::string name;
        std::vector<char> saveBytes;

        std::string get();

        std::vector<char> getBytes();

        ApsName(const std::vector<char> &loadBytes);
        ApsName(std::string name);
    };
} // namespace mpc::file::aps
