#include <file/AkaiNameGenerator.hpp>
#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::file;
using namespace moduru::lang;
using namespace std;

AkaiNameGenerator::AkaiNameGenerator(set<string> usedNames) 
{
	this->usedNames = usedNames;
}

bool AkaiNameGenerator::validChar(char toTest)
{
    if(toTest >= 'A' && toTest <= 'Z')
        return true;

    if(toTest >= '0' && toTest <= '9')
        return true;

    return (toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' || toTest == '!' || toTest == '#' || toTest == '%' || toTest == '&' || toTest == '-' || toTest == '{' || toTest == '}' || toTest == '(' || toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`');
}

bool AkaiNameGenerator::isSkipChar(char c)
{

	return (c == '.') || (c == ' ');
}

string AkaiNameGenerator::tidyString(string dirty)
{
	string result = "";
	for (int src = 0; src < dirty.length(); src++) {
		char toTest = toupper(dirty[src]);
		if (isSkipChar(toTest))
			continue;

		if (validChar(toTest)) {
			result.push_back(toTest);
		}
		else {
			result.push_back('_');
		}
	}
	return result;
}

bool AkaiNameGenerator::cleanString(string s)
{
	for (int i = 0; i < s.length(); i++) {
		if (isSkipChar(s[i]))
			return false;

		if (!validChar(s[i]))
			return false;

	}
	return true;
}

string AkaiNameGenerator::stripLeadingPeriods(string str)
{
	string s = "";
	for (int i = 0; i < str.length(); i++) {
		if (str[i] != '.') {
			s.append(str.substr(i));
			break;
		}
	}
	return s;
}

string AkaiNameGenerator::generateAkaiName(string longFullName)
{
	longFullName = StrUtil::toUpper(stripLeadingPeriods(longFullName));
	string longName;
	string longExt;
	int dotIdx = longFullName.find_last_of(".");
	if (dotIdx == string::npos) {
		longName = tidyString(longFullName);
		longExt = "";
	}
	else {
		cleanString(longFullName.substr(0, dotIdx));
		longName = tidyString(longFullName.substr(0, dotIdx));
		longExt = tidyString(longFullName.substr(dotIdx + 1));
	}
	auto const shortExt = (longExt.length() > 3) ? longExt.substr(0, 3) : longExt;
	if (longName.length() > 16 && (usedNames.find(StrUtil::toLower(longName.substr(0, 16) + "." + shortExt)) != usedNames.end() || usedNames.find(StrUtil::toLower(longName + "." + shortExt)) != usedNames.end())) {
		int maxLongIdx = longName.length() < 16 ? longName.length() : 16;
		for (int i = 1; i < 99999; i++) {
			string serial = "~" + to_string(i);
			int serialLen = serial.length();
			int trimIndex = maxLongIdx < 16 - serialLen ? maxLongIdx : 16 - serialLen;
			string shortName = longName.substr(0, trimIndex) + serial;
			if (usedNames.find(StrUtil::toLower(shortName + "." + shortExt)) == usedNames.end()) {
				return shortName + "." + shortExt;
			}
		}
		string error = "could not generate short name for " + longFullName;
	}
	if (longName.length() > 16)
		longName = longName.substr(0, 16);

	return longName + "." + shortExt;
}
