#include "file/aps/ApsAssignTable.hpp"

using namespace mpc::file::aps;

ApsAssignTable::ApsAssignTable(const std::vector<char> &loadBytes)
{
    assignTable = std::vector<int>(64);
    for (int i = 0; i < 64; i++)
    {
        assignTable[i] = loadBytes[i];
    }
}

ApsAssignTable::ApsAssignTable(const std::vector<DrumNoteNumber> &assignTable)
{
    saveBytes = std::vector<char>(Mpc2000XlSpecs::PROGRAM_PAD_COUNT);
    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        saveBytes[i] = assignTable[i];
    }
}

std::vector<int> ApsAssignTable::get()
{
    return assignTable;
}

std::vector<char> ApsAssignTable::getBytes()
{
    return saveBytes;
}
