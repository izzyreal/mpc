#pragma once
#include <synth/synths/multi/MultiMidiSynth.hpp>
#include <synth/synths/multi/MultiSynthControls.hpp>

#include <vector>
#include <memory>

namespace mpc {
	namespace ctootextensions {

		class Voice;

		class MpcMultiMidiSynth
			: public ctoot::synth::synths::multi::MultiMidiSynth
		{

		private:
			std::vector<std::shared_ptr<Voice>> voices;

		public:
			std::vector<std::weak_ptr<Voice>> getVoices();

		public:
			virtual void mpcTransportChannel(int track, ctoot::midi::core::MidiMessage* msg, int chan, int varType, int varValue, int l);

		public:
			virtual void mpcTransport(int track, ctoot::midi::core::MidiMessage* msg, int timestamp, int varType, int varValue, int l);

		public:
			virtual void mpcTransportChannel(int track, ctoot::midi::core::MidiMessage* msg, ctoot::synth::SynthChannel* synthChannel, int varType, int varValue, int l);
			std::weak_ptr<ctoot::synth::SynthChannel> mapChannel(int chan) override;

		public:
			MpcMultiMidiSynth(std::shared_ptr<ctoot::synth::synths::multi::MultiSynthControls> controls);
			~MpcMultiMidiSynth();

		};

	}
}
