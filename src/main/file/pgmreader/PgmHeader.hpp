#pragma once

#include <vector>

namespace mpc {
	namespace file {
		namespace pgmreader {

			class ProgramFileReader;

			class PgmHeader
			{

			private:
				std::vector<char> headerArray{};

			public:
				std::vector<char> getHeaderArray();
				bool verifyFirstTwoBytes();

			public:
				int getNumberOfSamples();

				PgmHeader(ProgramFileReader* programFile);
			};

		}
	}
}
