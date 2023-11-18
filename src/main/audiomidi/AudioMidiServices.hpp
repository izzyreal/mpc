#pragma once

#include <Mpc.hpp>
#include <sequencer/FrameSeq.hpp>

#include <engine/audio/mixer/AudioMixer.hpp>
#include "engine/MixerInterconnection.hpp"
#include "engine/Drum.hpp"
#include "engine/PreviewSoundPlayer.hpp"

#include <set>
#include <memory>
#include <atomic>

namespace mpc::engine {
	class Voice;
    class MixerInterconnection;
}

namespace mpc::engine::audio::core {
	class AudioFormat;
}

namespace mpc::engine::audio::server {
	class CompoundAudioClient;
	class NonRealTimeAudioServer;
	class AudioServer;
}

namespace mpc::engine::audio::mixer {
    class MixerControls;
}

namespace mpc::engine::audio::system {
	class DefaultAudioSystem;
}

namespace mpc::audiomidi {
	class MidiOutput;
	class DirectToDiskSettings;
	class DiskRecorder;
	class SoundRecorder;
	class SoundPlayer;
}

namespace mpc::audiomidi
{

	class AudioMidiServices final
	{

	private:
		mpc::Mpc& mpc;
		std::atomic<bool> bouncing = ATOMIC_VAR_INIT(false);
		std::atomic<bool> recordingSound = ATOMIC_VAR_INIT(false);
		std::shared_ptr<mpc::engine::audio::core::AudioProcess> monitorInputAdapter;

	public:
		const bool isBouncing();
		const bool isRecordingSound();
		void startBouncing();
		void stopBouncing();
		void stopBouncingEarly();
		void startRecordingSound();
		void stopSoundRecorder(bool cancel = false);

        // Should be called from the audio thread only!
        void changeSoundRecorderStateIfRequired();

        // Should be called from the audio thread only!
        void changeBounceStateIfRequired();

        // Should be called from the audio thread only!
        void switchMidiControlMappingIfRequired();

	private:
		bool bouncePrepared = false;
        bool wasRecordingSound = false;
        bool wasBouncing = false;
        std::vector<std::shared_ptr<mpc::engine::control::CompoundControl>> soundPlayerChannelControls;
        std::vector<mpc::engine::Drum> soundPlayerChannels;
        std::unique_ptr<mpc::engine::PreviewSoundPlayer> basicSoundPlayerChannel;
		std::vector<std::shared_ptr<mpc::engine::Voice>> voices;
        std::vector<mpc::engine::MixerInterconnection*> mixerConnections;
        std::shared_ptr<mpc::engine::Voice> basicVoice;
		std::shared_ptr<mpc::engine::audio::server::AudioServer> server;
		std::shared_ptr<mpc::engine::audio::server::NonRealTimeAudioServer> offlineServer;
		std::shared_ptr<mpc::engine::audio::system::DefaultAudioSystem> audioSystem;
		std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
		std::shared_ptr<mpc::engine::audio::mixer::MixerControls> mixerControls;
		std::shared_ptr<mpc::engine::audio::server::CompoundAudioClient> cac;
		std::shared_ptr<MidiOutput> midiOutput;
		std::vector<mpc::engine::audio::server::IOAudioProcess*> inputProcesses;
		std::vector<mpc::engine::audio::server::IOAudioProcess*> outputProcesses;
		std::shared_ptr<mpc::sequencer::FrameSeq> frameSeq;
		std::vector<std::shared_ptr<DiskRecorder>> diskRecorders;
		std::shared_ptr<SoundRecorder> soundRecorder;
		std::shared_ptr<SoundPlayer> soundPlayer;

	private:
		void setupMixer();
		void setAssignableMixOutLevels();
		void createSynth();
		std::vector<std::string> getInputNames();
		std::vector<std::string> getOutputNames();
		void setMonitorLevel(int level);

    public:
		mpc::engine::audio::server::NonRealTimeAudioServer* getAudioServer();

        std::shared_ptr<SoundRecorder> getSoundRecorder();
		std::shared_ptr<SoundPlayer> getSoundPlayer();
		void setMainLevel(int i);
		int getMainLevel();
        void setMixerMasterLevel(int8_t dbValue);
		void setRecordLevel(int i);
		int getRecordLevel();
		void muteMonitor(bool mute);
		mpc::engine::Drum& getDrum(int i);
        mpc::engine::PreviewSoundPlayer& getBasicPlayer();
		void initializeDiskRecorders();
		void closeIO();

	public:
		void connectVoices();
		std::shared_ptr<MidiOutput> getMidiOutput();
		void destroyServices();
		bool prepareBouncing(DirectToDiskSettings* settings);
		std::shared_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
		bool isBouncePrepared();

	public:
		void start(const int inputCount, const int outputCount);

	public:
		AudioMidiServices(mpc::Mpc& mpcToUse);
        ~AudioMidiServices();
	};
}
