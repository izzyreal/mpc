#pragma once
#include <controls/BaseControls.hpp>

#include <memory>

namespace ctoot {
	namespace mpc {
		class MpcStereoMixerChannel;
		class MpcIndivFxMixerChannel;
	}
}

namespace mpc {
	class Mpc;

	namespace ui {
		namespace sampler {
			class MixerGui;
			class MixerSetupGui;
		}
	}

	namespace controls {
		namespace mixer {

			class AbstractMixerControls
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				mpc::ui::sampler::MixerGui* mixerGui{ nullptr };
				mpc::ui::sampler::MixerSetupGui* mixerSetupGui{ nullptr };
				std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel> stereoMixerChannel;
				std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel> indivFxMixerChannel;

			protected:
				void init() override;

			public:
				AbstractMixerControls(mpc::Mpc* mpc);
				virtual ~AbstractMixerControls();

			};

		}
	}
}
