#pragma once

#include "IntTypes.hpp"

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

        explicit ApsAssignTable(const std::vector<char> &loadBytes);
        ApsAssignTable(const std::vector<DrumNoteNumber> &assignTable);
    };
} // namespace mpc::file::aps
