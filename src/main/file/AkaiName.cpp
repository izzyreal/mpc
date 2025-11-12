#include "AkaiName.hpp"
#include <StrUtil.hpp>

#include <algorithm>

#include <Logger.hpp>

using namespace mpc::file;

bool AkaiName::isAkaiName(const std::string &name)
{
    return name == generate(name);
}

std::vector<char> AkaiName::allowedSpecialCharacters = {
    '_', '^', '$', '~', '!', '#', '%',  '&',
    '-', '{', '}', '(', ')', '@', '\\', '`'};

bool AkaiName::isValidChar(const char &c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return true;
    }

    if (c >= '0' && c <= '9')
    {
        return true;
    }

    return find(allowedSpecialCharacters.begin(),
                allowedSpecialCharacters.end(),
                c) != allowedSpecialCharacters.end();
}

bool AkaiName::isCharThatShouldBeRemoved(const char &c)
{
    return c == '.' || c == ' ';
}

std::string AkaiName::cleanUpName(const std::string &name)
{
    std::string result = "";

    for (int src = 0; src < name.length(); src++)
    {
        char c = toupper(name[src]);

        if (isCharThatShouldBeRemoved(c))
        {
            continue;
        }

        if (isValidChar(c))
        {
            result.push_back(c);
        }
        else
        {
            result.push_back('_');
        }
    }

    return result;
}

std::string AkaiName::removeLeadingPeriod(const std::string &s)
{
    std::string result = "";
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] != '.')
        {
            result.append(s.substr(i));
            break;
        }
    }
    return s;
}

std::string AkaiName::generate(const std::string &name,
                               const std::vector<std::string> &usedNames)
{
    auto nameWithoutLeadingPeriod = StrUtil::toUpper(removeLeadingPeriod(name));

    std::string longName;
    std::string longExt;

    int dotIdx = nameWithoutLeadingPeriod.find_last_of(".");

    if (dotIdx == std::string::npos)
    {
        longName = cleanUpName(nameWithoutLeadingPeriod);
        longExt = "";
    }
    else
    {
        longName = cleanUpName(nameWithoutLeadingPeriod.substr(0, dotIdx));
        longExt = cleanUpName(nameWithoutLeadingPeriod.substr(dotIdx + 1));
    }

    auto shortExt = longExt.length() > 3 ? longExt.substr(0, 3) : longExt;
    shortExt = shortExt;

    longName = longName.substr(0, std::min((int)longName.length(), 16));

    auto firstCandidate = longName + "." + shortExt;

    if (find(usedNames.begin(), usedNames.end(), firstCandidate) !=
        usedNames.end())
    {

        int maxLongIdx = longName.length() < 16 ? longName.length() : 16;

        for (int i = 1; i < 99999; i++)
        {
            auto serial = "~" + std::to_string(i);
            int serialLen = serial.length();
            int trimIndex =
                maxLongIdx < 16 - serialLen ? maxLongIdx : 16 - serialLen;

            auto nameWithSerial = longName.substr(0, trimIndex) + serial;

            auto secondCandidate = nameWithSerial + "." + shortExt;

            if (find(usedNames.begin(), usedNames.end(), secondCandidate) ==
                usedNames.end())
            {
                return secondCandidate;
            }
        }
        std::string error = "could not generate short name for " + name;
        MLOG(error);
    }

    return firstCandidate;
}
