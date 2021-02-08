#include <file/aps/ApsName.hpp>

#include <file/aps/ApsParser.hpp>

#include <VecUtil.hpp>

using namespace mpc::file::aps;
using namespace moduru;
using namespace std;

ApsName::ApsName(const vector<char>& loadBytes)
{
	auto nameBytes = VecUtil::CopyOfRange(loadBytes, 0, NAME_STRING_LENGTH);
	name = "";
	for (char c : nameBytes) {
		if (c == 0x00) break;
		name.push_back(c);
	}
}

ApsName::ApsName(string name)
{
	saveBytes = vector<char>(ApsParser::APS_NAME_LENGTH);
	while (name.length() < NAME_STRING_LENGTH)
		name.push_back(' ');

	for (int i = 0; i < NAME_STRING_LENGTH; i++)
		saveBytes[i] = name[i];

	saveBytes[NAME_STRING_LENGTH] = ApsParser::NAME_TERMINATOR;
}

const int ApsName::NAME_STRING_LENGTH;

string ApsName::get()
{
    return name;
}

vector<char> ApsName::getBytes()
{
    return saveBytes;
}
