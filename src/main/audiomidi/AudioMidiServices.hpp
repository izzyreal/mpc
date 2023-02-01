#pragma once

#include <Mpc.hpp>
#include <sequencer/FrameSeq.hpp>

#include <audio/mixer/AudioMixer.hpp>

#include <observer/Observable.hpp>

#include <set>
#include <memory>
#include <atomic>

namespace ctoot::mpc {
	class MpcMixerControls;
	class MpcMultiMidiSynth;
	class MpcMultiSynthControls;
	class MpcVoice;
}

namespace ctoot::audio::core {
	class AudioFormat;
}

namespace ctoot::audio::server {
	class CompoundAudioClient;
	class NonRealTimeAudioServer;
	class AudioServer;
}

namespace ctoot::audio::system {
	class DefaultAudioSystem;
}

namespace ctoot::midi::core {
	class DefaultConnectedMidiSystem;
}

namespace ctoot::synth {
	class SynthRack;
	class SynthRackControls;
	class SynthChannelControls;
}

namespace mpc::audiomidi {
	class MpcMidiOutput;
	class DirectToDiskSettings;
	class DiskRecorder;
	class SoundRecorder;
	class SoundPlayer;
}

namespace mpc::audiomidi
{

	class AudioMidiServices final
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		std::atomic<bool> bouncing = ATOMIC_VAR_INIT(false);
		std::atomic<bool> recordingSound = ATOMIC_VAR_INIT(false);
		std::shared_ptr<ctoot::audio::core::AudioProcess> monitorInputAdapter;

	public:
		const bool isBouncing();
		const bool isRecordingSound();
		bool startBouncing();
		bool stopBouncing();
		bool stopBouncingEarly();
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
		std::vector<std::shared_ptr<ctoot::mpc::MpcVoice>> voices;
		std::shared_ptr<ctoot::mpc::MpcVoice> basicVoice;
		std::vector<std::shared_ptr<ctoot::synth::SynthChannelControls>> synthChannelControls;
		std::shared_ptr<ctoot::audio::server::AudioServer> server;
		std::shared_ptr<ctoot::audio::server::NonRealTimeAudioServer> offlineServer;
		std::shared_ptr<ctoot::audio::system::DefaultAudioSystem> audioSystem;
		std::shared_ptr<ctoot::midi::core::DefaultConnectedMidiSystem> midiSystem;
		std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
		std::shared_ptr<ctoot::synth::SynthRackControls> synthRackControls;
		std::shared_ptr<ctoot::synth::SynthRack> synthRack;
		std::shared_ptr<ctoot::mpc::MpcMultiSynthControls> msc;
		std::shared_ptr<ctoot::mpc::MpcMixerControls> mixerControls;
		std::shared_ptr<ctoot::mpc::MpcMultiMidiSynth> mms;
		std::shared_ptr<ctoot::audio::server::CompoundAudioClient> cac;
		std::shared_ptr<MpcMidiOutput> mpcMidiOutput;
		std::vector<std::shared_ptr<ctoot::audio::server::IOAudioProcess>> inputProcesses;
		std::vector<ctoot::audio::server::IOAudioProcess*> outputProcesses;
		std::shared_ptr<mpc::sequencer::FrameSeq> frameSeq;
		std::vector<std::shared_ptr<DiskRecorder>> diskRecorders;
		std::shared_ptr<SoundRecorder> soundRecorder;
		std::shared_ptr<SoundPlayer> soundPlayer;

	private:
		void destroySynth();
		void setupMixer();
		void setAssignableMixOutLevels();
		void createSynth();
		std::vector<std::string> getInputNames();
		std::vector<std::string> getOutputNames();
		void setMonitorLevel(int level);

    public:
		ctoot::audio::server::NonRealTimeAudioServer* getAudioServer();

        std::shared_ptr<SoundRecorder> getSoundRecorder();
		std::shared_ptr<SoundPlayer> getSoundPlayer();
		void setMainLevel(int i);
		int getMainLevel();
        void setMixerMasterLevel(int8_t dbValue);
		void setRecordLevel(int i);
		int getRecordLevel();
		void muteMonitor(bool mute);
		std::shared_ptr<ctoot::mpc::MpcMultiMidiSynth> getMms();
		void initializeDiskRecorders();
		void closeIO();

	public:
		void connectVoices();
		std::shared_ptr<MpcMidiOutput> getMidiPorts();
		void destroyServices();
		bool prepareBouncing(DirectToDiskSettings* settings);
		std::shared_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
		bool isBouncePrepared();

	public:
		void start(const int inputCount, const int outputCount);

	public:
		AudioMidiServices(mpc::Mpc& mpcToUse);

	};
}
