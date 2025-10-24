#include "file/aps/ApsAssignTable.hpp"

using namespace mpc::file::aps;

ApsAssignTable::ApsAssignTable(const std::vector<char>& loadBytes)
{
	assignTable = std::vector<int>(64);
	for (int i = 0; i < 64; i++)
		assignTable[i] = loadBytes[i];
}

ApsAssignTable::ApsAssignTable(std::vector<int> assignTable)
{
	saveBytes = std::vector<char>(64);
	for (int i = 0; i < 64; i++)
		saveBytes[i] = assignTable[i];
}

std::vector<int> ApsAssignTable::get()
{
    return assignTable;
}

std::vector<char> ApsAssignTable::getBytes()
{
    return saveBytes;
}
