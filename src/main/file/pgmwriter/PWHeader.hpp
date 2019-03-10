#pragma once

#include <vector>

namespace mpc {
	namespace file {
		namespace pgmwriter {

			class PWHeader
			{

			private:
				std::vector<char> headerArray{};

			private:
				void writeFirstTwoBytes();
				void setNumberOfSamples(int numberOfSamples);

			public:
				std::vector<char> getHeaderArray();

			public:
				PWHeader(int numberOfSamples);
			};

		}
	}
}
