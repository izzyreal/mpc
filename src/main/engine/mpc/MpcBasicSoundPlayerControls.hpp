#pragma once
#include <engine/audio/mixer/AudioMixer.hpp>

#include <memory>

namespace ctoot::mpc {

		class MpcSampler;
		class MpcVoice;

		class MpcBasicSoundPlayerControls
			: public ctoot::control::CompoundControl
		{

		private:
			static std::string NAME_;

        private:
			std::shared_ptr<ctoot::mpc::MpcSampler> sampler;
			std::shared_ptr<ctoot::mpc::MpcVoice> voice;
			std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;

		public:
			std::shared_ptr<ctoot::mpc::MpcSampler> getSampler();
			std::shared_ptr<ctoot::audio::mixer::AudioMixer> getMixer();
			std::shared_ptr<ctoot::mpc::MpcVoice> getVoice();

			MpcBasicSoundPlayerControls(std::shared_ptr<ctoot::mpc::MpcSampler> sampler, std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer, std::shared_ptr<ctoot::mpc::MpcVoice> voice);

		public:
			static std::string& NAME();

		};
	}
