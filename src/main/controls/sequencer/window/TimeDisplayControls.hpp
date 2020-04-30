#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class TimeDisplayControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;
					void turnWheel(int i) override;

					TimeDisplayControls();

				};

			}
		}
	}
}
