#pragma once

#include <vector>

namespace mpc {
	namespace file {
		namespace aps {

			class ApsDrumConfiguration
			{

			public:
				int program{};
				bool receivePgmChange{};
				bool receiveMidiVolume{};
				std::vector<char> saveBytes{};

			private:
				static std::vector<char> TEMPLATE;
				static std::vector<char> PADDING;

			public:
				int getProgram();
				bool getReceivePgmChange();
				bool getReceiveMidiVolume();

			public:
				std::vector<char> getBytes();

			public:
				ApsDrumConfiguration(std::vector<char> loadBytes);
				ApsDrumConfiguration(int program, bool recPgmChange, bool recMidiVolume);
			};

		}
	}
}
