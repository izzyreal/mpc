#include "AkaiName.hpp"
#include <lang/StrUtil.hpp>

#include <cmath>
#include <algorithm>

#include <Logger.hpp>

using namespace mpc::file;
using namespace moduru::lang;
using namespace std;

bool AkaiName::isAkaiName(const string& name)
{
	return name.compare(generate(name)) == 0;
}

vector<char> AkaiName::allowedSpecialCharacters = { '_', '^', '$', '~', '!', '#', '%', '&', '-', '{', '}', '(', ')', '@', '\\', '`' };

bool AkaiName::isValidChar(const char& c)				  
{																  
	if (c >= 'A' && c <= 'Z') {									  
		return true;											  
	}															  
																  
	if (c >= '0' && c <= '9') {									 
		return true;											 
	}															  
																  
	return find (
		AkaiName::allowedSpecialCharacters.begin(),
		AkaiName::allowedSpecialCharacters.end(),
		c
	) != AkaiName::allowedSpecialCharacters.end();
}

bool AkaiName::isCharThatShouldBeRemoved(const char& c)
{
	return (c == '.') || (c == ' ');
}

string AkaiName::cleanUpName(const string& name)
{
	string result = "";
	
	for (int src = 0; src < name.length(); src++) {
		char c = toupper(name[src]);
		
		if (isCharThatShouldBeRemoved(c)) {
			continue;
		}

		if (isValidChar(c)) {
			result.push_back(c);
		}
		else {
			result.push_back('_');
		}
	}

	return result;
}

string AkaiName::removeLeadingPeriod(const string& s)
{
	string result = "";
	for (int i = 0; i < s.length(); i++) {
		if (s[i] != '.') {
			result.append(s.substr(i));
			break;
		}
	}
	return s;
}

string AkaiName::generate(const string& name, const vector<string>& usedNames)
{
	auto nameWithoutLeadingPeriod = StrUtil::toUpper(removeLeadingPeriod(name));
	
	string longName;
	string longExt;
	
	int dotIdx = nameWithoutLeadingPeriod.find_last_of(".");
	
	if (dotIdx == string::npos) {
		longName = cleanUpName(nameWithoutLeadingPeriod);
		longExt = "";
	}
	else {
		longName = cleanUpName(nameWithoutLeadingPeriod.substr(0, dotIdx));
		longExt = cleanUpName(nameWithoutLeadingPeriod.substr(dotIdx + 1));
	}

	auto shortExt = (longExt.length() > 3) ? longExt.substr(0, 3) : longExt;
	shortExt = shortExt;

	longName = longName.substr(0, std::min((int)longName.length(), 16));

	auto firstCandidate = longName + "." + shortExt;

	if ( find(usedNames.begin(), usedNames.end(), firstCandidate) != usedNames.end() ) {
		
		int maxLongIdx = longName.length() < 16 ? longName.length() : 16;

		for (int i = 1; i < 99999; i++) {
			auto serial = "~" + to_string(i);
			int serialLen = serial.length();
			int trimIndex = maxLongIdx < 16 - serialLen ? maxLongIdx : 16 - serialLen;
			
			auto nameWithSerial = longName.substr(0, trimIndex) + serial;
			
			auto secondCandidate = nameWithSerial + "." + shortExt;

			if  ( find (usedNames.begin(), usedNames.end(), secondCandidate) == usedNames.end() ) {
				return secondCandidate;
			}
		}
		string error = "could not generate short name for " + name;
		MLOG(error);
	}

	return firstCandidate;
}
