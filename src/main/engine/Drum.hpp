#pragma once

#include <map>
#include <vector>
#include <memory>
#include "MixerInterconnection.hpp"

namespace mpc::lcdgui::screens {
    class MixerSetupScreen;
}

namespace mpc::sampler {
    class Sampler;
    class NoteParameters;
}

namespace mpc::engine::audio::mixer {
	class AudioMixer;
}

namespace mpc::engine {
	class StereoMixer;
	class IndivFxMixer;
	class Voice;
	class MixerInterconnection;
}
namespace mpc::sequencer {
	class NoteOnEvent;
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
		std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
		std::vector<MixerInterconnection*>& mixerConnections;
        mpc::lcdgui::screens::MixerSetupScreen* mixerSetupScreen = nullptr;

	private:
		int drumIndex = 0;
		int programNumber = 0;
		bool receivePgmChange = false;
		bool receiveMidiVolume = false;
		int lastReceivedMidiVolume = 127;
		std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels;
		std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels;

	public:
		int getProgram();
		void allNotesOff();
		void setProgram(int i);
		bool receivesPgmChange();
		void setReceivePgmChange(bool b);
		bool receivesMidiVolume();
		void setReceiveMidiVolume(bool b);
		void setLastReceivedMidiVolume(int volume);
		int getLastReceivedMidiVolume();
		void mpcNoteOn(int note, int velo, int varType, int varValue, int frameOffset, bool firstGeneration, int startTick, int durationFrames);
		//void mpcNoteOn(std::shared_ptr<mpc::sequencer::NoteOnEvent>& event, int frameOffset, bool firstGeneration, int durationFrames);

	public:
		void allSoundOff(int frameOffset);
		std::vector<std::shared_ptr<StereoMixer>>& getStereoMixerChannels();
		std::vector<std::shared_ptr<IndivFxMixer>>& getIndivFxMixerChannels();

	public:
		void mpcNoteOff(int note, int frameOffset, int noteOnStartTick);

	private:
        void checkForMutes(mpc::sampler::NoteParameters* np);
        void startDecayForNote(const int note,
                               const int frameOffset,
                               const int noteOnStartTick);
        void stopMonoOrPolyVoiceWithSameNoteParameters(mpc::sampler::NoteParameters* noteParameters, int note);

	public:
		Drum(std::shared_ptr<mpc::sampler::Sampler> sampler,
             int drumIndex,
             std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
             mpc::lcdgui::screens::MixerSetupScreen* mixerSetupScreen,
             std::vector<std::shared_ptr<Voice>> voices,
             std::vector<mpc::engine::MixerInterconnection*>& mixerConnections);

	};
}
