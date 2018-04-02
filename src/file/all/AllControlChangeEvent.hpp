#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace file {
		namespace all {

			class AllControlChangeEvent
			{

			private:
				static int CONTROLLER_OFFSET;
				static int AMOUNT_OFFSET;

			public:
				mpc::sequencer::Event* event {  };
				std::vector<char> saveBytes{};

			public:
				AllControlChangeEvent(std::vector<char> ba);
				AllControlChangeEvent(mpc::sequencer::Event* e);
			};

		}
	}
}
