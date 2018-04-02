#pragma once

#include <string>
#include <vector>

namespace mpc {
	namespace file {
		namespace pgmreader {

			class ProgramFileReader;

			class SoundNames
			{

			public:
				std::vector<char> sampleNamesArray{};

			private:
				ProgramFileReader* programFile{};

			public:
				int getSampleNamesSize();
				std::vector<char> getSampleNamesArray();

			public:
				std::string getSampleName(int sampleNumber);

				SoundNames(ProgramFileReader* programFile);
			};

		}
	}
}
