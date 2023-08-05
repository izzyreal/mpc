#pragma once

#include <vector>
#include <string>

namespace mpc::disk
{
	class ShortName;
}

namespace mpc::disk
{
	class ShortNameGenerator
	{

	private:
		std::vector<std::string> usedNames;

	public:
		static bool validChar(char toTest);
		static bool isSkipChar(char c);

	private:
		bool cleanString(std::string s);
		std::string stripLeadingPeriods(std::string str);

	public:
		ShortName generateShortName(const std::string& longFullName);
        static std::string tidyString(std::string dirty);

		ShortNameGenerator(const std::vector<std::string>& usedNames);
	};
}
