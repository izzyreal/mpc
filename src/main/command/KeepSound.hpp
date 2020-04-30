#pragma once

#include <memory>

namespace mpc {

	

	namespace sequencer {
		class Sequencer;
		class Track;
	}
	namespace sampler {
		class Sampler;
		class Sound;
	}

	namespace ui {
		namespace sampler {
			class SamplerGui;
			class SoundGui;
		}
	}

	namespace command {

		class KeepSound
		{

		private:
			
			std::weak_ptr<mpc::sampler::Sound> sound{};
			mpc::ui::sampler::SamplerGui* samplerGui{ nullptr };
			mpc::ui::sampler::SoundGui* soundGui{ nullptr };
		
		public:
			void execute();

			KeepSound(std::weak_ptr<mpc::sampler::Sound> s);

		};

	}
}
