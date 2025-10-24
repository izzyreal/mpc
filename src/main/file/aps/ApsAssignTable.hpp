#pragma once

#include <vector>

namespace mpc::file::aps
{
    class ApsAssignTable
    {

    public:
        std::vector<int> assignTable;
        std::vector<char> saveBytes;

        std::vector<int> get();
        std::vector<char> getBytes();

        ApsAssignTable(const std::vector<char> &loadBytes);
        ApsAssignTable(std::vector<int> assignTable);
    };
} // namespace mpc::file::aps
