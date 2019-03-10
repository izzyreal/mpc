#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace dialog {

				class MetronomeSoundControls
					: public mpc::controls::sequencer::AbstractSequencerControls
				{

				public:
					typedef mpc::controls::sequencer::AbstractSequencerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

					MetronomeSoundControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
