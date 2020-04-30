#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class SequenceControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

					SequenceControls();

				};

			}
		}
	}
}
