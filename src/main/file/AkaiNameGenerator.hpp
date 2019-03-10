#pragma once

#include <string>
#include <set>

namespace mpc {
	namespace file {

		class AkaiNameGenerator
		{

		private:
			std::set<std::string> usedNames;

		public:
			static bool validChar(char toTest);
			static bool isSkipChar(char c);

		private:
			std::string tidyString(std::string dirty);
			bool cleanString(std::string s);
			std::string stripLeadingPeriods(std::string str);

		public:
			std::string generateAkaiName(std::string longFullName);

			AkaiNameGenerator(std::set<std::string> usedNames);
		};

	}
}
