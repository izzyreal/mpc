#pragma once

#include <vector>

namespace mpc {

	namespace sampler {
		class Program;
	}

	namespace file {
		namespace pgmwriter {

			class Slider
			{

			public:
				std::vector<char> sliderArray{};

			public:
				std::vector<char> getSliderArray();

			private:
				void setMidiNoteAssign(int midiNoteAssign);
				void setTuneLow(int tuneLow);
				void setTuneHigh(int tuneHigh);
				void setDecayLow(int decayLow);
				void setDecayHigh(int decayHigh);
				void setAttackLow(int attackLow);
				void setAttackHigh(int attackHigh);
				void setFilterLow(int filterLow);
				void setFilterHigh(int filterHigh);
				void setControlChange(int controlChange);

			public:
				Slider(mpc::sampler::Program* program);
			};

		}
	}
}
