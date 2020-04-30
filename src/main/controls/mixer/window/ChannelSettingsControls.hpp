#pragma once
#include <controls/mixer/AbstractMixerControls.hpp>

namespace mpc {
	namespace controls {
		namespace mixer {
			namespace window {

				class ChannelSettingsControls
					: public mpc::controls::mixer::AbstractMixerControls
				{

				public:
					typedef mpc::controls::mixer::AbstractMixerControls super;
					void turnWheel(int increment) override;

				public:
					ChannelSettingsControls();
					~ChannelSettingsControls();

				};

			}
		}
	}
}
