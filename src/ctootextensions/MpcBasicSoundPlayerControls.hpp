#pragma once
#include <synth/SynthChannelControls.hpp>
#include <audio/mixer/AudioMixer.hpp>

#include <memory>

namespace mpc {

	namespace sampler {
		class Sampler;
	}

	namespace ctootextensions {

		class Voice;
		class MpcSoundOscillatorControls;
		class MpcEnvelopeControls;

		class MpcBasicSoundPlayerControls
			: public ctoot::synth::SynthChannelControls
		{

		public:
			typedef ctoot::synth::SynthChannelControls super;

		public:
			static const int MPC_BASIC_SOUND_PLAYER_CHANNEL_ID{ 9 };

		private:
			static std::string NAME_;

		public:
			static const int OSC_OFFSET{ 0 };
			static const int AMP_OFFSET{ 56 };
			static const int AMPENV_OFFSET{ 64 };

		private:
			std::weak_ptr<mpc::sampler::Sampler> sampler{ };
			std::weak_ptr<ctoot::audio::mixer::AudioMixer> mixer{};
			std::weak_ptr<mpc::ctootextensions::Voice> voice{};

		public:
			std::weak_ptr<mpc::sampler::Sampler> getSampler();
			std::weak_ptr<ctoot::audio::mixer::AudioMixer> getMixer();
			std::weak_ptr<mpc::ctootextensions::Voice> getVoice();

			MpcBasicSoundPlayerControls(std::weak_ptr<mpc::sampler::Sampler> sampler, std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer, std::weak_ptr<mpc::ctootextensions::Voice> voice);
			~MpcBasicSoundPlayerControls();

		public:
			static std::string& NAME();

		};

	}
}
