#pragma once

#include <vector>
#include <string>

namespace mpc {
	namespace file {
		namespace aps {

			class ApsName
			{

			private:
				static const int NAME_STRING_LENGTH{ 16 };

			public:
				std::string name{};
				std::vector<char> saveBytes{};

			public:
				std::string get();

			public:
				std::vector<char> getBytes();

				ApsName(std::vector<char> loadBytes);
				ApsName(std::string name);
			};

		}
	}
}
