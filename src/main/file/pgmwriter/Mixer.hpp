#pragma once

#include <vector>

namespace mpc {

	namespace sampler {
		class Program;
	}

	namespace file {
		namespace pgmwriter {

			class Mixer
			{

			public:
				std::vector<char> mixerArray{};

			public:
				std::vector<char> getMixerArray();

			private:
				void setEffectsOutput(int pad, int effectsOutputNumber);
				void setVolume(int pad, int volume);
				void setPan(int pad, int pan);
				void setVolumeIndividual(int pad, int volumeIndividual);
				void setOutput(int pad, int output);
				void setEffectsSendLevel(int pad, int effectsSendLevel);

			public:
				Mixer(mpc::sampler::Program* program);
			};

		}
	}
}
