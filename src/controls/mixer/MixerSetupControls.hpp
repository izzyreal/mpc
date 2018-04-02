#pragma once
#include <controls/mixer/AbstractMixerControls.hpp>

namespace mpc {
	namespace controls {
		namespace mixer {

			class MixerSetupControls
				: public AbstractMixerControls
			{

			public:
				typedef AbstractMixerControls super;
				void turnWheel(int increment) override;
				void function(int i) override;

			public:
				MixerSetupControls(mpc::Mpc* mpc);
				~MixerSetupControls();

			};

		}
	}
}
