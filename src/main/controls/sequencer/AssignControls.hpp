#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	class Mpc;
	namespace controls {
		namespace sequencer {

			class AssignControls
				: public AbstractSequencerControls
			{

				typedef AbstractSequencerControls super;

			public:
				void turnWheel(int i) override;
				void pad(int i, int velo, bool repeat, int tick) override;

				AssignControls(mpc::Mpc* mpc);

			};

		}
	}
}
