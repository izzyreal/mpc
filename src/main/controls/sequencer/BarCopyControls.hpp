#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {

			class BarCopyControls
				: public AbstractSequencerControls
			{

			public:
				typedef AbstractSequencerControls super;
				void function(int j) override;
				void turnWheel(int i) override;

				BarCopyControls(mpc::Mpc* mpc);

			};

		}
	}
}
