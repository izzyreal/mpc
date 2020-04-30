#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class EditMultipleControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;
					void function(int i) override;
					void turnWheel(int i) override;

				private:
					void checkThreeParameters();
					void checkFiveParameters();
					void checkNotes();

				public:
					EditMultipleControls();

				};

			}
		}
	}
}
