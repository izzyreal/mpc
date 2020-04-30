#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace dialog {

				class DeleteAllSequencesControls
					: public mpc::controls::sequencer::AbstractSequencerControls
				{

				public:
					typedef mpc::controls::sequencer::AbstractSequencerControls super;
					void function(int i) override;

					DeleteAllSequencesControls();

				};

			}
		}
	}
}
