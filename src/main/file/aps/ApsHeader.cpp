#include <file/aps/ApsHeader.hpp>

using namespace mpc::file::aps;

ApsHeader::ApsHeader(const std::vector<char>& loadBytes)
{
	valid = loadBytes[0] == 10 && loadBytes[1] == 5 && loadBytes[3] == 0;
	soundCount = loadBytes[2];
}

ApsHeader::ApsHeader(int soundCount)
{
	saveBytes = std::vector<char>(4);
	saveBytes[0] = 10;
	saveBytes[1] = 5;
	saveBytes[2] = soundCount;
	saveBytes[3] = 0;
}

bool ApsHeader::isValid()
{
    return valid;
}

int ApsHeader::getSoundAmount()
{
    return soundCount;
}

std::vector<char> ApsHeader::getBytes()
{
    return saveBytes;
}
