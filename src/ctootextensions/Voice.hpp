#pragma once

#include <audio/core/AudioProcess.hpp>

namespace ctoot {
	namespace control {
		class FloatControl;
		class BooleanControl;
	}
	namespace synth {
		namespace modules {
			namespace filter {
				class StateVariableFilter;
				class StateVariableFilterControls;
			}
		}
	}
}

namespace mpc {
	
	namespace sampler {
		class NoteParameters;
		class Sound;
	}

	namespace ctootextensions {

		class MpcEnvelopeGenerator;
		class MpcEnvelopeControls;
		class MpcSoundPlayerChannel;
		class MuteInfo;

		class Voice
			: public ctoot::audio::core::AudioProcess
		{

		private:
			float hostSampleRate = 44100.0;
			float timeRatio;
			
			static const float STATIC_ATTACK_LENGTH;
			static const float STATIC_DECAY_LENGTH;
			static const int MAX_ATTACK_LENGTH_MS{ 3000 };
			static const int MAX_DECAY_LENGTH_MS{ 2600 };
			static const int MAX_ATTACK_LENGTH_SAMPLES{ 132300 };
			static const int MAX_DECAY_LENGTH_SAMPLES{ 114660 };
			static const int ATTACK_INDEX{ 0 };
			static const int HOLD_INDEX{ 1 };
			static const int DECAY_INDEX{ 2 };
			static const int RESO_INDEX{ 1 };
			static const int MIX_INDEX{ 2 };
			static const int BANDPASS_INDEX{ 3 };

			std::weak_ptr<mpc::sampler::Sound> oscVars{};
			int tune{ 0 };
			double increment{ 0 };
			double position{ 0 };
			std::vector<float>* sampleData;
			int padNumber{ -1 };
			mpc::ctootextensions::MpcEnvelopeGenerator* staticEnv{ nullptr };
			mpc::ctootextensions::MpcEnvelopeGenerator* ampEnv{ nullptr };
			mpc::ctootextensions::MpcEnvelopeGenerator* filterEnv{ nullptr };
			float initialFilterValue{ 0 };
			bool staticDecay{ 0 };
			mpc::sampler::NoteParameters* np{ 0 };
			float amplitude{ 0 };
			ctoot::synth::modules::filter::StateVariableFilter* svf0{ nullptr };
			int start{ 0 };
			int end{ 0 };
			ctoot::control::FloatControl* attack{ nullptr };
			ctoot::control::FloatControl* hold{ nullptr };
			ctoot::control::FloatControl* decay{ nullptr };
			ctoot::control::FloatControl* fattack{ nullptr };
			ctoot::control::FloatControl* fhold{ nullptr };
			ctoot::control::FloatControl* fdecay{ nullptr };
			ctoot::control::FloatControl* sattack{ nullptr };
			ctoot::control::FloatControl* shold{ nullptr };
			ctoot::control::FloatControl* sdecay{ nullptr };
			ctoot::control::FloatControl* reso{ nullptr };
			ctoot::control::FloatControl* mix{ nullptr };
			ctoot::control::BooleanControl* bandpass{ nullptr };
			mpc::ctootextensions::MpcEnvelopeControls* ampEnvControls{ nullptr };
			mpc::ctootextensions::MpcEnvelopeControls* staticEnvControls{ nullptr };
			mpc::ctootextensions::MpcEnvelopeControls* filterEnvControls{ nullptr };
			ctoot::synth::modules::filter::StateVariableFilterControls* svfControls{ nullptr };
			bool finished{ true };
			bool readyToPlay{ false };
			int stripNumber{ -1 };

		public:
			float inverseNyquist{ 2.f / 44100.f };

		private:
			void readFrame();

		private:
			ctoot::synth::modules::filter::StateVariableFilter* svf1{ nullptr };
			mpc::ctootextensions::MpcSoundPlayerChannel* parent{ nullptr };
			mpc::ctootextensions::MuteInfo* muteInfo{ nullptr };
			int track{ 0 };
			int frameOffset{ 0 };
			bool basic{ false };
			int decayCounter{ 0 };
			bool enableEnvs{ false };
			static const int SVF_OFFSET{ 48 };
			static const int AMPENV_OFFSET{ 64 };
			std::vector<float> EMPTY_FRAME;
			std::vector<float> tempFrame;
			float veloFactor{ 0 };
			int veloToStart{ 0 };
			int attackValue{ 0 };
			int decayValue{ 0 };
			int veloToAttack{ 0 };
			int decayMode{ 0 };
			int veloToLevel{ 0 };
			int playableSampleLength{ 0 };
			float attackMs{ 0 };
			int finalDecayValue{ 0 };
			float decayMs{ 0 };
			int attackLengthSamples{ 0 };
			int decayLengthSamples{ 0 };
			int holdLengthSamples{ 0 };
			int staticEnvHoldSamples{ 0 };
			float veloToLevelFactor{ 0 };
			int filtParam{ 0 };
			float envAmplitude{ 0 };
			float staticEnvAmp{ 0 };
			double frac{ 0 };
			int k{ 0 };
			int j{ 0 };
			std::vector<float> frame;

		public:
			bool isFinished();
			void init(int track, int velocity, int padNumber, std::weak_ptr<mpc::sampler::Sound> oscVars, mpc::sampler::NoteParameters* np, int varType, int varValue, int muteNote, int muteDrum, int frameOffset, bool enableEnvs);
			std::vector<float> getFrame();

			int getPadNumber();
			void open();
			int processAudio(ctoot::audio::core::AudioBuffer* buffer) override;
			void close();
			void startDecay();
			int getVoiceOverlap();
			int getStripNumber();
			bool isDecaying();
			mpc::ctootextensions::MuteInfo* getMuteInfo();
			void startDecay(int offset);
			void finish();

		public:
			Voice(int stripNumber, bool basic, int sampleRate);
			~Voice();
		};

	}
}
