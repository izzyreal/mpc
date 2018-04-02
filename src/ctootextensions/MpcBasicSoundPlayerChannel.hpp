#pragma once
#include <synth/SynthChannel.hpp>
#include <audio/mixer/AudioMixerStrip.hpp>

#include <memory>

namespace mpc {

	namespace sampler {
		class Sampler;
		class Sound;
	}

	namespace ctootextensions {

		class MpcBasicSoundPlayerControls;
		class Voice;

		class MpcBasicSoundPlayerChannel
			: public ctoot::synth::SynthChannel
		{

		public:
			typedef ctoot::synth::SynthChannel super;

		private:
			std::weak_ptr<mpc::sampler::Sampler> sampler{};
			std::weak_ptr<mpc::ctootextensions::Voice> voice{};
			std::weak_ptr<ctoot::audio::mixer::AudioMixerStrip> mixerStrip{ };
			std::weak_ptr<ctoot::audio::mixer::AudioMixer> mixer{ };
			std::weak_ptr<ctoot::audio::fader::FaderControl> fader;
			std::weak_ptr<mpc::sampler::Sound> tempVars{};

		public:
			void finishVoice();
			void setLocation(std::string location) override;
			void noteOn(int soundNumber, int velocity) override;
			void mpcNoteOn(int soundNumber, int velocity, int frameOffset);
			void noteOff(int note) override;
			void allNotesOff() override;
			void allSoundOff() override;
			void connectVoice();

			MpcBasicSoundPlayerChannel(std::weak_ptr<MpcBasicSoundPlayerControls> controls);
			~MpcBasicSoundPlayerChannel();

		public:
			void noteOff(int pitch, int velocity);

		};

	}
}
