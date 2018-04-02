#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class TimingCorrectControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

					TimingCorrectControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
