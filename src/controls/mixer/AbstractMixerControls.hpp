#pragma once
#include <controls/AbstractControls.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace sampler {
		class StereoMixerChannel;
		class IndivFxMixerChannel;
	}

	namespace ui {
		namespace sampler {
			class MixerGui;
			class MixerSetupGui;
		}
	}

	namespace controls {
		namespace mixer {

			class AbstractMixerControls
				: public mpc::controls::AbstractControls
			{

			public:
				typedef mpc::controls::AbstractControls super;

			protected:
				mpc::ui::sampler::MixerGui* mixerGui{ nullptr };
				mpc::ui::sampler::MixerSetupGui* mixerSetupGui{ nullptr };
				std::weak_ptr<mpc::sampler::StereoMixerChannel> stereoMixerChannel;
				std::weak_ptr<mpc::sampler::IndivFxMixerChannel> indivFxMixerChannel;

			protected:
				void init() override;

			public:
				AbstractMixerControls(mpc::Mpc* mpc);
				virtual ~AbstractMixerControls();

			};

		}
	}
}
