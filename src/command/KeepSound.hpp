#pragma once

#include <memory>

namespace mpc {

	class Mpc;

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
			mpc::Mpc* mpc{};
			std::weak_ptr<mpc::sampler::Sound> sound{};
			mpc::ui::sampler::SamplerGui* samplerGui{ nullptr };
			mpc::ui::sampler::SoundGui* soundGui{ nullptr };
		
		public:
			void execute();

			KeepSound(mpc::Mpc* mpc, std::weak_ptr<mpc::sampler::Sound> s);

		};

	}
}
