#include "file/aps/ApsName.hpp"

#include "file/aps/ApsParser.hpp"

#include "Util.hpp"

using namespace mpc::file::aps;

ApsName::ApsName(const std::vector<char> &loadBytes)
{
    auto nameBytes = Util::vecCopyOfRange(loadBytes, 0, NAME_STRING_LENGTH);
    name = "";
    for (char c : nameBytes)
    {
        if (c == 0x00)
        {
            break;
        }
        name.push_back(c);
    }
}

ApsName::ApsName(std::string name)
{
    saveBytes = std::vector<char>(ApsParser::APS_NAME_LENGTH);
    while (name.length() < NAME_STRING_LENGTH)
    {
        name.push_back(' ');
    }

    for (int i = 0; i < NAME_STRING_LENGTH; i++)
    {
        saveBytes[i] = name[i];
    }

    saveBytes[NAME_STRING_LENGTH] = ApsParser::NAME_TERMINATOR;
}

const int ApsName::NAME_STRING_LENGTH;

std::string ApsName::get()
{
    return name;
}

std::vector<char> ApsName::getBytes()
{
    return saveBytes;
}
