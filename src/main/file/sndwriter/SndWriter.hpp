#pragma once

#include <vector>
#include <string>

namespace mpc {

	namespace sampler {
		class Sound;
	}

	namespace file {
		namespace sndwriter {

			class SndHeaderWriter;

			class SndWriter
			{

			private:
				static const int HEADER_SIZE{ 42 };
				SndHeaderWriter* sndHeaderWriter{};
				mpc::sampler::Sound* sound{};
				std::vector<char> sndFileArray{};

			private:
				void setValues();
				void setName(std::string s);
				void setMono(bool b);
				void setFramesCount(int i);
				void setSampleRate(int i);
				void setLevel(int i);
				void setStart(int i);
				void setEnd(int i);
				void setLoopLength(int i);
				void setLoopEnabled(bool b);
				void setTune(int i);
				void setBeatCount(int i);
				void setSampleData(std::vector<float> fa, bool b);

			public:
				std::vector<char> getSndFileArray();

				SndWriter(mpc::sampler::Sound* sound);
			};

		}
	}
}
