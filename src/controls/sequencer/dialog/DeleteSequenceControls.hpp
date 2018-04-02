#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace dialog {

				class DeleteSequenceControls
					: public mpc::controls::sequencer::AbstractSequencerControls
				{

				public:
					typedef mpc::controls::sequencer::AbstractSequencerControls super;
					void turnWheel(int i) override;
					void function(int i) override;

					DeleteSequenceControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
