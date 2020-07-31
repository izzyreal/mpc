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
	class MpcMidiPorts;
	class DirectToDiskSettings;
	class DiskRecorder;
	class SoundRecorder;
	class SoundPlayer;
}

using namespace mpc::audiomidi;

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
		void startBouncing();
		void stopBouncing();
		void startRecordingSound();
		void stopSoundRecorder();
		
	private:
		bool bouncePrepared = false;
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
		std::weak_ptr<ctoot::mpc::MpcMultiMidiSynth> mms;
		std::shared_ptr<ctoot::audio::server::CompoundAudioClient> cac;
		std::shared_ptr<MpcMidiPorts> mpcMidiPorts;
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
		std::vector<std::weak_ptr<DiskRecorder>> getDiskRecorders();
		std::weak_ptr<SoundRecorder> getSoundRecorder();
		std::weak_ptr<SoundPlayer> getSoundPlayer();
		void setMasterLevel(int i);
		int getMasterLevel();
		void setRecordLevel(int i);
		int getRecordLevel();
		void muteMonitor(bool mute);
		std::weak_ptr<ctoot::mpc::MpcMultiMidiSynth> getMms();
		void initializeDiskRecorders();
		void closeIO();

	public:
		void connectVoices();
		std::weak_ptr<MpcMidiPorts> getMidiPorts();
		void destroyServices();
		void prepareBouncing(DirectToDiskSettings* settings);
		std::weak_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
		bool isBouncePrepared();

	public:
		void start(const int sampleRate, const int inputCount, const int outputCount);

	public:
		AudioMidiServices(mpc::Mpc& mpc);

	};
}
