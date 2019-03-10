#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class Assign16LevelsControls
					: public AbstractSequencerControls
				{

				public:
					typedef mpc::controls::sequencer::AbstractSequencerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

					Assign16LevelsControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
