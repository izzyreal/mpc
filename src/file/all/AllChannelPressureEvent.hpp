#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace file {
		namespace all {

			class AllChannelPressureEvent
			{

			private:
				static int AMOUNT_OFFSET;

			public:
				mpc::sequencer::Event* event {  };
				std::vector<char> saveBytes{};

			public:
				AllChannelPressureEvent(std::vector<char> ba);
				AllChannelPressureEvent(mpc::sequencer::Event* e);
			};

		}
	}
}
