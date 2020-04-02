#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>
#include <audio/server/AudioServer.hpp>
#include <audio/mixer/AudioMixer.hpp>
#include <sequencer/FrameSeq.hpp>

#include <observer/Observable.hpp>

#include <set>
#include <memory>

namespace ctoot {

	namespace mpc {
		class MpcMixerControls;
		class MpcMultiMidiSynth;
		class MpcMultiSynthControls;
		class MpcVoice;
	}

	namespace audio {

		namespace core {
			class AudioFormat;
		}

		namespace server {
			class NonRealTimeAudioServer;
			class CompoundAudioClient;
			class ExternalAudioServer;
			class UnrealAudioServer;
		}

		namespace system {
			class DefaultAudioSystem;
		}
	}

	namespace midi {
		namespace core {
			class DefaultConnectedMidiSystem;
		}
	}

	namespace synth {
		class SynthRack;
		class SynthRackControls;
		class SynthChannelControls;
	}
}

namespace mpc {

	namespace audiomidi {

		class MpcMidiPorts;
		class DirectToDiskSettings;
		class ExportAudioProcessAdapter;

		class AudioMidiServices final
			: public moduru::observer::Observable
		{

		private:
			bool bouncePrepared{ false };
			bool bouncing{ false };

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
			Mpc* mpc{ nullptr };
			std::vector<std::shared_ptr<ExportAudioProcessAdapter>> exportProcesses;
			std::vector<ctoot::audio::server::IOAudioProcess*> inputProcesses;
			std::vector<ctoot::audio::server::IOAudioProcess*> outputProcesses;
			std::shared_ptr<mpc::sequencer::FrameSeq> frameSeq;
			std::vector<int> oldPrograms;

		public:
			ctoot::audio::server::UnrealAudioServer* getUnrealAudioServer();
			ctoot::audio::server::ExternalAudioServer* getExternalAudioServer();

		private:
			void destroySynth();
			void destroyDiskWriter();
			void setupMidi();
			void setupMixer();
			void setAssignableMixOutLevels();
			void createSynth();

			void setupFX();

		public:
			std::weak_ptr<ctoot::audio::server::AudioServer> getAudioServer();
			ctoot::audio::server::NonRealTimeAudioServer* getOfflineServer();
			void setMasterLevel(int i);
			int getMasterLevel();
			void setRecordLevel(int i);
			int getRecordLevel();
			std::vector<std::string> getInputNames();
			std::vector<std::string> getOutputNames();
			std::weak_ptr<ctoot::mpc::MpcMultiMidiSynth> getMms();
			void initializeDiskWriter();
			void closeIO();

		public:
			void connectVoices();
			std::weak_ptr<MpcMidiPorts> getMidiPorts();
			void destroyServices();
			void prepareBouncing(DirectToDiskSettings* settings);
			void startBouncing();
			void stopBouncing();
			std::weak_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();
			bool isBouncePrepared();
			bool isBouncing();
			ctoot::audio::server::IOAudioProcess* getAudioInput(int input);
			int getBufferSize();

		public:
			void start(const int sampleRate, const int inputCount, const int outputCount);

		public:
			AudioMidiServices(Mpc* mpc);
			~AudioMidiServices();

		};

	}
}
