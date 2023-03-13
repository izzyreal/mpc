#pragma once

#include <map>
#include <vector>
#include <memory>

namespace ctoot::control {
	class CompoundControl;
}

namespace ctoot::audio::server {
	class AudioServer;
}

namespace ctoot::audio::mixer {
	class AudioMixer;
	class MainMixControls;
}

namespace ctoot::mpc {
	class MpcSampler;
	class MpcStereoMixerChannel;
	class MpcIndivFxMixerChannel;
	class MpcProgram;
	class MpcNoteParameters;
	class MpcSound;
	class MpcVoice;
	class MpcSoundPlayerControls;
	class MpcFaderControl;
	class MpcMixerInterconnection;
}

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel final
	{

	private:
		std::map<int, int> simultA;
		std::map<int, int> simultB;
		std::shared_ptr<MpcSoundPlayerControls> controls;
        std::vector<std::shared_ptr<MpcVoice>> voices;
		std::shared_ptr<MpcSampler> sampler;
		std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
		std::vector<MpcMixerInterconnection*> mixerConnections;
		ctoot::audio::server::AudioServer* server = nullptr;

	private:
		int drumIndex = 0;
		int programNumber = 0;
		bool receivePgmChange = false;
		bool receiveMidiVolume = false;
		int lastReceivedMidiVolume = 127;
		std::vector<std::shared_ptr<MpcStereoMixerChannel>> stereoMixerChannels;
        std::vector<std::shared_ptr<MpcStereoMixerChannel>> weakStereoMixerChannels;
		std::vector<std::shared_ptr<MpcIndivFxMixerChannel>> indivFxMixerChannels;
        std::vector<std::shared_ptr<MpcIndivFxMixerChannel>> weakIndivFxMixerChannels;

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
		std::vector<std::shared_ptr<MpcStereoMixerChannel>>& getStereoMixerChannels();
		std::vector<std::shared_ptr<MpcIndivFxMixerChannel>>& getIndivFxMixerChannels();

	public:
		void mpcNoteOff(int note, int frameOffset, int noteOnStartTick);

	private:
        void checkForMutes(MpcNoteParameters* np);
        void startDecayForNote(const int note,
                               const int frameOffset,
                               const int noteOnStartTick);
        void stopMonoOrPolyVoiceWithSameNoteParameters(ctoot::mpc::MpcNoteParameters* noteParameters, int note);

	public:
		MpcSoundPlayerChannel(std::shared_ptr<MpcSoundPlayerControls> controls);
		~MpcSoundPlayerChannel();

	};
}
