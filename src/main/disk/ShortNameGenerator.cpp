#include "ShortNameGenerator.hpp"

#include "ShortName.hpp"

#include "StrUtil.hpp"

#include <algorithm>

using namespace mpc::disk;
using namespace std;

ShortNameGenerator::ShortNameGenerator(const vector<string> &usedNames)
{
    this->usedNames = usedNames;
}

bool ShortNameGenerator::validChar(char toTest)
{
    if (toTest >= 'A' && toTest <= 'Z')
    {
        return true;
    }

    if (toTest >= '0' && toTest <= '9')
    {
        return true;
    }

    return toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' ||
           toTest == '!' || toTest == '#' || toTest == '%' || toTest == '&' ||
           toTest == '-' || toTest == '{' || toTest == '}' || toTest == '(' ||
           toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`';
}

bool ShortNameGenerator::isSkipChar(char c)
{
    return c == '.' || c == ' ';
}

string ShortNameGenerator::tidyString(const string &dirty)
{
    string result = "";
    for (int src = 0; src < dirty.length(); src++)
    {
        char toTest = toupper(dirty[src]);
        if (isSkipChar(toTest))
        {
            continue;
        }

        if (validChar(toTest))
        {
            result.push_back(toTest);
        }
        else
        {
            result.push_back('_');
        }
    }
    return result;
}

bool ShortNameGenerator::cleanString(const string &s) const
{
    for (auto i = 0; i < s.length(); i++)
    {
        if (isSkipChar(s[i]))
        {
            return false;
        }

        if (!validChar(s[i]))
        {
            return false;
        }
    }
    return true;
}

string ShortNameGenerator::stripLeadingPeriods(const string &str) const
{
    string sb = "";
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] != '.')
        {
            sb = str.substr(i);
            break;
        }
    }
    return sb;
}

ShortName ShortNameGenerator::generateShortName(const string &longFullName)
{
    auto name = StrUtil::toUpper(stripLeadingPeriods(longFullName));
    string longName;
    string longExt;
    size_t dotIdx = name.find_last_of('.');
    bool forceSuffix = false;
    if (dotIdx == string::npos)
    {
        forceSuffix = !cleanString(name);
        longName = tidyString(name);
        longExt = "";
    }
    else
    {
        forceSuffix = !cleanString(name.substr(0, dotIdx));
        longName = tidyString(name.substr(0, dotIdx));
        longExt = tidyString(name.substr(dotIdx + 1));
    }
    string shortExt = longExt.length() > 3 ? longExt.substr(0, 3) : longExt;

    if (forceSuffix || longName.length() > 8 ||
        find(usedNames.begin(), usedNames.end(),
             ShortName(longName, shortExt).asSimpleString()) != usedNames.end())
    {

        auto const maxLongIdx =
            static_cast<int>(longName.length() < 8 ? longName.length() : 8);

        for (int i = 1; i < 99999; i++)
        {

            string serial = "~" + to_string(i);
            int serialLen = static_cast<int>(serial.length());
            int trimIndex =
                maxLongIdx < 8 - serialLen ? maxLongIdx : 8 - serialLen;

            auto const shortName = longName.substr(0, trimIndex) + serial;
            ShortName result(shortName, shortExt);

            if (find(usedNames.begin(), usedNames.end(),
                     result.asSimpleString()) == usedNames.end())
            {
                return result;
            }
        }

        string error = "Short name generation ran out of serials for " +
                       longFullName + ", returning original!";
        return longFullName;
    }

    return ShortName(longName, shortExt);
}
