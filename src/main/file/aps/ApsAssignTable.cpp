#include <file/aps/ApsAssignTable.hpp>

using namespace mpc::file::aps;
using namespace std;

ApsAssignTable::ApsAssignTable(const vector<char>& loadBytes) 
{
	assignTable = vector<int>(64);
	for (int i = 0; i < 64; i++)
		assignTable[i] = loadBytes[i];
}

ApsAssignTable::ApsAssignTable(vector<int> assignTable) 
{
	saveBytes = vector<char>(64);
	for (int i = 0; i < 64; i++)
		saveBytes[i] = assignTable[i];
}

vector<int> ApsAssignTable::get()
{
    return assignTable;
}

vector<char> ApsAssignTable::getBytes()
{
    return saveBytes;
}
