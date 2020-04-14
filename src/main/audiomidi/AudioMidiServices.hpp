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
}

using namespace mpc::audiomidi;

namespace mpc::audiomidi {

	class AudioMidiServices final
		: public moduru::observer::Observable
	{

	private:
		atomic<bool> bouncing = ATOMIC_VAR_INIT(false);
		atomic<bool> recordingSound = ATOMIC_VAR_INIT(false);
		
	public:
		const bool isBouncing();
		const bool isRecordingSound();
		void startBouncing();
		void stopBouncing();
		void startRecordingSound();
		void stopSoundRecorder();
		
	private:
		bool bouncePrepared = false;
		vector<shared_ptr<ctoot::mpc::MpcVoice>> voices;
		shared_ptr<ctoot::mpc::MpcVoice> basicVoice;
		vector<shared_ptr<ctoot::synth::SynthChannelControls>> synthChannelControls;
		shared_ptr<ctoot::audio::server::AudioServer> server;
		shared_ptr<ctoot::audio::server::NonRealTimeAudioServer> offlineServer;
		shared_ptr<ctoot::audio::system::DefaultAudioSystem> audioSystem;
		shared_ptr<ctoot::midi::core::DefaultConnectedMidiSystem> midiSystem;
		shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
		shared_ptr<ctoot::synth::SynthRackControls> synthRackControls;
		shared_ptr<ctoot::synth::SynthRack> synthRack;
		shared_ptr<ctoot::mpc::MpcMultiSynthControls> msc;
		shared_ptr<ctoot::mpc::MpcMixerControls> mixerControls;
		weak_ptr<ctoot::mpc::MpcMultiMidiSynth> mms;
		shared_ptr<ctoot::audio::server::CompoundAudioClient> cac;
		shared_ptr<MpcMidiPorts> mpcMidiPorts;
		Mpc* mpc{ nullptr };
		vector<shared_ptr<ctoot::audio::server::IOAudioProcess>> inputProcesses;
		vector<ctoot::audio::server::IOAudioProcess*> outputProcesses;
		shared_ptr<mpc::sequencer::FrameSeq> frameSeq;
		vector<int> oldPrograms;
		vector<shared_ptr<DiskRecorder>> diskRecorders;
		shared_ptr<SoundRecorder> soundRecorder;

	private:
		void destroySynth();
		void setupMixer();
		void setAssignableMixOutLevels();
		void createSynth();
		vector<string> getInputNames();
		vector<string> getOutputNames();
		void setMonitorLevel(int level);

	public:
		ctoot::audio::server::NonRealTimeAudioServer* getAudioServer();
		vector<weak_ptr<DiskRecorder>> getDiskRecorders();
		weak_ptr<SoundRecorder> getSoundRecorder();
		void setMasterLevel(int i);
		int getMasterLevel();
		void setRecordLevel(int i);
		int getRecordLevel();
		void muteMonitor(bool mute);
		weak_ptr<ctoot::mpc::MpcMultiMidiSynth> getMms();
		void initializeDiskRecorders();
		void closeIO();

	public:
		void connectVoices();
		weak_ptr<MpcMidiPorts> getMidiPorts();
		void destroyServices();
		void prepareBouncing(DirectToDiskSettings* settings);
		weak_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
		bool isBouncePrepared();

	public:
		void start(const int sampleRate, const int inputCount, const int outputCount);

	public:
		AudioMidiServices(Mpc* mpc);
		~AudioMidiServices();

	};
}
