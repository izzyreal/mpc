#pragma once

#include <vector>
#include <string>

namespace mpc {

	namespace sequencer {
		class Song;
	}

	namespace file {
		namespace all {

			class Song
			{

			public:
				static const int LENGTH{ 528 };

			private:
				static const int NAME_OFFSET{ 0 };

			public:
				std::string name{};
				std::vector<char> saveBytes{};

			public:
				std::string getName();

			public:
				std::vector<char> getBytes();

				Song(std::vector<char> b);
				Song(mpc::sequencer::Song* mpcSong);
			};

		}
	}
}
