#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace file {
		namespace all {

			class AllPitchBendEvent
			{

			private:
				static int AMOUNT_OFFSET;

			public:
				mpc::sequencer::Event* event;
				std::vector<char> saveBytes;

			public:
				AllPitchBendEvent(const std::vector<char>& ba);
				AllPitchBendEvent(mpc::sequencer::Event* e);

			};
		}
	}
}
