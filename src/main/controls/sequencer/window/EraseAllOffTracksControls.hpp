#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class EraseAllOffTracksControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;
					void function(int i) override;

					EraseAllOffTracksControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
