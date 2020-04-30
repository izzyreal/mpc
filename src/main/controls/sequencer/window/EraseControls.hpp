#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sequencer {
			namespace window {

				class EraseControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;

				private:
					std::vector<std::string> eventClassNames{};

				public:
					void turnWheel(int i) override;
					void function(int i) override;

					EraseControls();

				};

			}
		}
	}
}
