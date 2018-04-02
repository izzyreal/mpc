#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace file {
		namespace all {

			class AllPolyPressureEvent
			{

			private:
				static int NOTE_OFFSET;
				static int AMOUNT_OFFSET;

			public:
				mpc::sequencer::Event* event {  };
				std::vector<char> saveBytes{};

			public:
				AllPolyPressureEvent(std::vector<char> ba);
				AllPolyPressureEvent(mpc::sequencer::Event* e);
			};

		}
	}
}
