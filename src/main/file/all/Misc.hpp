#pragma once

#include <memory>
#include <vector>

namespace mpc {
	class Mpc;
	namespace file {
		namespace all {

			class Misc
			{

			public:
				static const int LENGTH{ 128 };

			private:
				static const int TAP_AVG_OFFSET{ 33 };
				static const int MIDI_SYNC_IN_RECEIVE_MMC_OFFSET{ 34 };
				static const int AUTO_STEP_INCREMENT_OFFSET{ 46 };
				static const int DURATION_OF_REC_NOTES_OFFSET{ 47 };
				static const int DURATION_TC_PERCENTAGE_OFFSET{ 48 };
				static const int MIDI_PGM_CHANGE_TO_SEQ_OFFSET{ 49 };

			public:
				int tapAvg{};
				bool inReceiveMMCEnabled{};
				bool autoStepInc{};
				bool durationOfRecNotesTcEnabled{};
				int durationTcPercentage{};
				bool pgmChToSeqEnabled{};
				std::vector<char> saveBytes{};

			public:
				int getTapAvg();
				bool isInReceiveMMCEnabled();
				bool isAutoStepIncEnabled();
				bool isDurationOfRecNotesTc();
				int getDurationTcPercentage();
				bool isPgmChToSeqEnabled();

			public:
				std::vector<char> getBytes();

			public:
				Misc(std::vector<char> b);
				Misc(mpc::Mpc* mpc);
			};

		}
	}
}
