#pragma once
#include <synth/SynthChannelControls.hpp>
#include <audio/server/AudioServer.hpp>
#include <audio/mixer/AudioMixer.hpp>

#include <memory>

namespace mpc {

	namespace ui {
		namespace sampler {
			class MixerSetupGui;
		}
	}

	namespace sampler {
		class Sampler;
	}

	namespace ctootextensions {
		
		class Voice;
		class MpcMultiMidiSynth;

		class MpcSoundPlayerControls
			: public ctoot::synth::SynthChannelControls
		{

		private:
			std::weak_ptr<MpcMultiMidiSynth> mms;

		public:
			typedef ctoot::synth::SynthChannelControls super;

		public:
			static const int MPC_SOUND_PLAYER_CHANNEL_ID{ 8 };

		private:
			static std::string NAME_;
			std::weak_ptr<mpc::sampler::Sampler> sampler{};
			std::weak_ptr<ctoot::audio::mixer::AudioMixer> mixer{};
			int drumNumber{ 0 };
			std::weak_ptr<ctoot::audio::server::AudioServer> server{ };
			mpc::ui::sampler::MixerSetupGui* mixerSetupGui;
			
		public:
			std::weak_ptr<MpcMultiMidiSynth> getMms();
			std::weak_ptr<mpc::sampler::Sampler> getSampler();
			int getDrumNumber();
			std::weak_ptr<ctoot::audio::mixer::AudioMixer> getMixer();
			ctoot::audio::server::AudioServer* getServer();
			mpc::ui::sampler::MixerSetupGui* getMixerSetupGui();

		public:
			MpcSoundPlayerControls(std::weak_ptr<MpcMultiMidiSynth> mms, std::weak_ptr<mpc::sampler::Sampler> sampler, int drumNumber, std::weak_ptr<ctoot::audio::mixer::AudioMixer> mixer, std::weak_ptr<ctoot::audio::server::AudioServer> server, mpc::ui::sampler::MixerSetupGui* mixerSetupGui);
			~MpcSoundPlayerControls();

		public:
			static std::string NAME();

		};

	}
}
