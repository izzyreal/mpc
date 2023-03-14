#pragma once

#include <map>
#include <vector>
#include <memory>

namespace mpc::engine::audio::server {
	class AudioServer;
}

namespace mpc::engine::audio::mixer {
	class AudioMixer;
}

namespace mpc::engine {
	class MpcSampler;
	class StereoMixer;
	class IndivFxMixer;
	class MpcNoteParameters;
	class Voice;
	class MixerInterconnection;
    class MpcMixerSetupGui;
}

namespace mpc::engine
{
	class Drum final
	{

	private:
		std::map<int, int> simultA;
		std::map<int, int> simultB;
        std::vector<std::shared_ptr<Voice>> voices;
		std::shared_ptr<MpcSampler> sampler;
		std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
		std::vector<MixerInterconnection*> mixerConnections;
		mpc::engine::audio::server::AudioServer* server = nullptr;
        mpc::engine::MpcMixerSetupGui* mixerSetupGui = nullptr;

	private:
		int drumIndex = 0;
		int programNumber = 0;
		bool receivePgmChange = false;
		bool receiveMidiVolume = false;
		int lastReceivedMidiVolume = 127;
		std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels;
        std::vector<std::shared_ptr<StereoMixer>> weakStereoMixerChannels;
		std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels;
        std::vector<std::shared_ptr<IndivFxMixer>> weakIndivFxMixerChannels;

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

	public:
		void allSoundOff(int frameOffset);
		void connectVoices();
		std::vector<std::shared_ptr<StereoMixer>>& getStereoMixerChannels();
		std::vector<std::shared_ptr<IndivFxMixer>>& getIndivFxMixerChannels();

	public:
		void mpcNoteOff(int note, int frameOffset, int noteOnStartTick);

	private:
        void checkForMutes(MpcNoteParameters* np);
        void startDecayForNote(const int note,
                               const int frameOffset,
                               const int noteOnStartTick);
        void stopMonoOrPolyVoiceWithSameNoteParameters(mpc::engine::MpcNoteParameters* noteParameters, int note);

	public:
		Drum(std::shared_ptr<MpcSampler> sampler,
             int drumIndex,
             std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
             const std::shared_ptr<mpc::engine::audio::server::AudioServer>& server,
             MpcMixerSetupGui *mixerSetupGui,
             std::vector<std::shared_ptr<Voice>> voices);
		~Drum();

	};
}
