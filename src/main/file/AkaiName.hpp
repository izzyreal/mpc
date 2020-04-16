#pragma once

#include <string>
#include <vector>

using namespace std;

namespace mpc::file {

	class AkaiName
	{
	private:
		static vector<char> allowedSpecialCharacters;

	private:
		static bool isValidChar(const char& toTest);
		static bool isCharThatShouldBeRemoved(const char& c);

	private:
		static string cleanUpName(const string& name);
		static string removeLeadingPeriod(const string& str);

	public:
		static bool isAkaiName(const string& name);

	public:
		static string generate(const string& name, const vector<string>& usedNames = vector<string>());

	};
}
