#pragma once

#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace sequencer {
		class Sequence;
	}

	namespace file {
		namespace all {

			class SequenceNames
			{

			public:
				static const int LENGTH{ 1782 };
				static const int ENTRY_LENGTH{ 18 };
				static const int LAST_EVENT_INDEX_OFFSET{ 16 };
				std::vector<std::string> names = std::vector<std::string>(99);
				std::vector<char> saveBytes{};

			public:
				std::vector<std::string> getNames();

			public:
				std::vector<char> getBytes();
				static int getSegmentCount(mpc::sequencer::Sequence* seq);

				SequenceNames(std::vector<char> b);
				SequenceNames(mpc::Mpc* mpc);
			};

		}
	}
}
