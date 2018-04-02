#pragma once

#include <vector>
#include <string>

namespace mpc {
	namespace file {
		namespace sndwriter {
			
			class SndWriter;

			class SndHeaderWriter
			{

			private:
				std::vector<char> headerArray{};

			public:
				std::vector<char> getHeaderArray();
				void setFirstTwoBytes();
				void setName(std::string s);
				void setLevel(int i);
				void setTune(int i);
				void setMono(bool b);
				void setStart(int i);
				void setEnd(int i);
				void setFrameCount(int i);
				void setLoopLength(int i);
				void setLoopEnabled(bool b);
				void setBeatCount(int i);
				void setSampleRate(int i);

			private:
				void putLE(int offset, int value);

			public:
				SndHeaderWriter(SndWriter* sndWriter);
			};

		}
	}
}
