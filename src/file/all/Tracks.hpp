#pragma once

#include <vector>
#include <string>

namespace mpc {

	namespace sequencer {
		class Sequence;
	}

	namespace file {
		namespace all {

			class Tracks
			{

			private:
				static const int TRACK_NAMES_OFFSET{ 0 };
				static const int BUSSES_OFFSET{ 1088 };
				static const int PGMS_OFFSET{ 1152 };
				static const int VELO_RATIOS_OFFSET{ 1216 };
				static const int STATUS_OFFSET{ 1280 };
				static const int PADDING1_OFFSET{ 1408 };
				static std::vector<char> PADDING1;
				static const int LAST_TICK_BYTE1_OFFSET{ 1424 };
				static const int LAST_TICK_BYTE2_OFFSET{ 1425 };
				static const int LAST_TICK_BYTE3_OFFSET{ 1426 };
				static const int UNKNOWN32_BIT_INT_OFFSET{ 1428 };
				std::vector<int> busses = std::vector<int>(64);
				std::vector<int> veloRatios = std::vector<int>(64);
				std::vector<int> pgms = std::vector<int>(64);
				std::vector<std::string> names = std::vector<std::string>(64);
				std::vector<int> status = std::vector<int>(64);

			public:
				std::vector<char> saveBytes{};

			public:
				int getBus(int i);
				int getVelo(int i);
				int getPgm(int i);
				std::string getName(int i);
				int getStatus(int i);

			public:
				std::vector<char> getBytes();

				Tracks(std::vector<char> loadBytes);
				Tracks(mpc::sequencer::Sequence* seq);
			};

		}
	}
}
