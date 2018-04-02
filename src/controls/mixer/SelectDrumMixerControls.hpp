#pragma once
#include <controls/mixer/AbstractMixerControls.hpp>

namespace mpc {
	namespace controls {
		namespace mixer {

			class SelectDrumMixerControls
				: public AbstractMixerControls
			{

			public:
				typedef AbstractMixerControls super;
				void function(int i) override;

			public:
				SelectDrumMixerControls(mpc::Mpc* mpc);
				~SelectDrumMixerControls();

			};

		}
	}
}
