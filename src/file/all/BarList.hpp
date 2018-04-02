#pragma once

#include <vector>

namespace mpc {

	namespace sequencer {
		class Sequence;
	}

	namespace file {
		namespace all {

			class Bar;

			class BarList
			{


			public:
				std::vector<Bar*> bars{};
				std::vector<char> saveBytes{};

			public:
				std::vector<Bar*> getBars();

			public:
				std::vector<char> getBytes();
				BarList(std::vector<char> loadBytes);

			public:
				BarList(mpc::sequencer::Sequence* seq);
			};

		}
	}
}
