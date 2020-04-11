#pragma once

#include <Mpc.hpp>
#include <sequencer/FrameSeq.hpp>
#include "SamplerAudioIO.hpp"

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
	class ExportAudioProcessAdapter;
}

using namespace mpc::audiomidi;

namespace mpc::audiomidi {

	class AudioMidiServices final
		: public moduru::observer::Observable
	{

	private:
		bool bouncePrepared{ false };
		atomic<bool> bouncing = ATOMIC_VAR_INIT(false);
		// Whenever we're in the "Sample" screen, we're sampling, regardless of whether we're recording
		atomic<bool> sampling = ATOMIC_VAR_INIT(false);

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
		vector<ctoot::audio::server::IOAudioProcess*> inputProcesses;
		vector<ctoot::audio::server::IOAudioProcess*> outputProcesses;
		shared_ptr<mpc::sequencer::FrameSeq> frameSeq;
		vector<int> oldPrograms;

	private:
		vector<shared_ptr<ExportAudioProcessAdapter>> exportProcesses;

	private:
		void destroySynth();
		void destroyDiskWriter();
		void setupMidi();
		void setupMixer();
		void setAssignableMixOutLevels();
		void createSynth();

		void setupFX();

	public:
		ctoot::audio::server::NonRealTimeAudioServer* getAudioServer();
		vector<weak_ptr<ExportAudioProcessAdapter>> getExportProcesses();
		void setMasterLevel(int i);
		int getMasterLevel();
		void setRecordLevel(int i);
		int getRecordLevel();
		vector<string> getInputNames();
		vector<string> getOutputNames();
		weak_ptr<ctoot::mpc::MpcMultiMidiSynth> getMms();
		void initializeDiskWriter();
		void closeIO();

	public:
		void connectVoices();
		weak_ptr<MpcMidiPorts> getMidiPorts();
		void destroyServices();
		void prepareBouncing(DirectToDiskSettings* settings);
		void startBouncing();
		void stopBouncing();
		weak_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
		bool isBouncePrepared();
		const bool isBouncing();
		ctoot::audio::server::IOAudioProcess* getAudioInput(int input);
		int getBufferSize();

	public:
		void start(const int sampleRate, const int inputCount, const int outputCount);

	public:
		AudioMidiServices(Mpc* mpc);
		~AudioMidiServices();

	};
}
