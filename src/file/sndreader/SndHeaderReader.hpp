#pragma once

#include <string>
#include <vector>

namespace mpc {
	namespace file {
		namespace sndreader {

			class SndReader;

			class SndHeaderReader
			{

			private:
				std::vector<char> headerArray{};
			
			public:
				std::vector<char>* getHeaderArray();
				bool verifyFirstTwoBytes();
				std::string getName();
				int getLevel();
				int getTune();
				bool isMono();
				int getStart();
				int getEnd();
				int getNumberOfFrames();
				int getLoopLength();
				bool isLoopEnabled();
				int getNumberOfBeats();
				int getSampleRate();

			public:
				SndHeaderReader(SndReader* sndReader);

			};

		}
	}
}
