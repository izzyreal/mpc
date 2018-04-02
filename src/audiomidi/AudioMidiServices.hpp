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
	namespace audio {

		namespace core {
			class AudioFormat;
		}

		namespace server {
			class NonRealTimeAudioServer;
			class CompoundAudioClient;
			class RtAudioServer;
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

	namespace ctootextensions {
		class MpcMixerControls;
		class MpcMultiMidiSynth;
		class MpcMultiSynthControls;
		class Voice;
	}

	namespace audiomidi {

		class MpcMidiPorts;
		class DirectToDiskSettings;
		class ExportAudioProcessAdapter;

		class AudioMidiServices
			: public moduru::observer::Observable
		{

		private:
			bool disabled{ true };
			bool bouncePrepared{ false };
			bool bouncing{ false };

			std::shared_ptr<ctoot::audio::core::AudioFormat> format;
			std::vector<std::shared_ptr<mpc::ctootextensions::Voice>> voices;
			std::shared_ptr<mpc::ctootextensions::Voice> basicVoice;
			std::vector<std::shared_ptr<ctoot::synth::SynthChannelControls>> synthChannelControls;
			std::shared_ptr<ctoot::audio::server::AudioServer> server;
			std::shared_ptr<ctoot::audio::server::NonRealTimeAudioServer> offlineServer;
			std::shared_ptr<ctoot::audio::system::DefaultAudioSystem> audioSystem;
			std::shared_ptr<ctoot::midi::core::DefaultConnectedMidiSystem> midiSystem;
			std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
			std::shared_ptr<ctoot::synth::SynthRackControls> synthRackControls;
			std::shared_ptr<ctoot::synth::SynthRack> synthRack;
			std::shared_ptr<ctootextensions::MpcMultiSynthControls> msc;
			std::shared_ptr<ctootextensions::MpcMixerControls> mixerControls;
			std::weak_ptr<ctootextensions::MpcMultiMidiSynth> mms;
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
			ctoot::audio::server::RtAudioServer* getRtAudioServer();

		private:
			void destroySynth();
			void destroyDiskWriter();
			void setupMidi();
			void setupMixer();
			void setAssignableMixOutLevels();
			void createSynth(int sampleRate);

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
			std::weak_ptr<ctootextensions::MpcMultiMidiSynth> getMms();
			void initializeDiskWriter();
			void closeIO();
			void disable();

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
			bool isDisabled();
			ctoot::audio::server::IOAudioProcess* getAudioInput(int input);
			int getBufferSize();

		public:
			void start(std::string mode, int sampleRate);
			void setDisabled(bool b);

		public:
			AudioMidiServices(Mpc* mpc);
			~AudioMidiServices();

		};

	}
}
