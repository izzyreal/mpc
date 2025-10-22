#pragma once

#include <map>
#include <vector>
#include <memory>

namespace mpc::sampler {
    class Sampler;
    class NoteParameters;
}

namespace mpc::engine {
	class StereoMixer;
	class IndivFxMixer;
	class Voice;
}

namespace mpc::engine
{
	class Drum final
	{

	private:
		std::map<int, int> simultA;
		std::map<int, int> simultB;
        std::vector<std::shared_ptr<Voice>> voices;
		std::shared_ptr<mpc::sampler::Sampler> sampler;

	private:
		int drumIndex = 0;
		int programNumber = 0;
		bool receivePgmChange = false;
		bool receiveMidiVolume = false;
		int lastReceivedMidiVolume = 127;
		std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels;
		std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels;

	public:
        int getIndex() { return drumIndex; }
		int getProgram();
		void allNotesOff();
		void setProgram(int i);
		bool receivesPgmChange();
		void setReceivePgmChange(bool b);
		bool receivesMidiVolume();
		void setReceiveMidiVolume(bool b);
		void setLastReceivedMidiVolume(int volume);
		int getLastReceivedMidiVolume();

        std::map<int, int>& getSimultA() { return simultA; }
        std::map<int, int>& getSimultB() { return simultB; }

	public:
		void allSoundOff(int frameOffset);
		std::vector<std::shared_ptr<StereoMixer>>& getStereoMixerChannels();
		std::vector<std::shared_ptr<IndivFxMixer>>& getIndivFxMixerChannels();

	private:
        void checkForMutes(mpc::sampler::NoteParameters* np);
        void startDecayForNote(const int note,
                               const int frameOffset,
                               const int noteOnStartTick);
        void stopMonoOrPolyVoiceWithSameNoteParameters(mpc::sampler::NoteParameters* noteParameters, int note);

	public:
		Drum(std::shared_ptr<mpc::sampler::Sampler>,
             const int drumIndex,
             std::vector<std::shared_ptr<Voice>>&);
	};
}

