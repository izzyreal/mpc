#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace file {
		namespace all {

			class AllProgramChangeEvent
			{

			private:
				static int PROGRAM_OFFSET;

			public:
				mpc::sequencer::Event* event {  };
				std::vector<char> saveBytes{};

			public:
				AllProgramChangeEvent(std::vector<char> ba);
				AllProgramChangeEvent(mpc::sequencer::Event* e);
			};

		}
	}
}
