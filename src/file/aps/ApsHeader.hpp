#pragma once

#include <vector>

namespace mpc {
	namespace file {
		namespace aps {

			class ApsHeader
			{

			public:
				bool valid{};
				int soundAmount{};
				std::vector<char> saveBytes{};

			public:
				bool isValid();
				int getSoundAmount();
				std::vector<char> getBytes();

				ApsHeader(std::vector<char> loadBytes);
				ApsHeader(int soundCount);
			};

		}
	}
}
