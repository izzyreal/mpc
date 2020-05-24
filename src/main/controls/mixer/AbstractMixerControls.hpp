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

	namespace controls {
		namespace mixer {

			class AbstractMixerControls
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel> stereoMixerChannel;
				std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel> indivFxMixerChannel;

			protected:
				void init() override;

			public:
				AbstractMixerControls();

			};

		}
	}
}
