#pragma once

#include <memory>
#include <vector>

namespace mpc::file::all {

	class MidiInput
	{

	private:
		static std::vector<char> TEMPLATE;

	public:
		static const int LENGTH{ 48 };
		static const int RECEIVE_CH_OFFSET{ 3 };
		static const int SUSTAIN_PEDAL_TO_DURATION_OFFSET{ 4 };
		static const int FILTER_ENABLED_OFFSET{ 5 };
		static const int FILTER_TYPE_OFFSET{ 6 };
		static const int MULTI_REC_ENABLED_OFFSET{ 7 };
		static const int MULTI_REC_TRACK_DESTS_OFFSET{ 8 };
		static const int MULTI_REC_TRACK_DESTS_LENGTH{ 34 };
		static const int NOTE_PASS_ENABLED_OFFSET{ 42 };
		static const int PITCH_BEND_PASS_ENABLED_OFFSET{ 43 };
		static const int PGM_CHANGE_PASS_ENABLED_OFFSET{ 44 };
		static const int CH_PRESSURE_PASS_ENABLED_OFFSET{ 45 };
		static const int POLY_PRESSURE_PASS_ENABLED_OFFSET{ 46 };
		static const int EXCLUSIVE_PASS_ENABLED_OFFSET{ 47 };
		int receiveCh{};
		bool sustainPedalToDuration{};
		bool filterEnabled{};
		int filterType{};
		bool multiRecEnabled{};
		std::vector<int> multiRecTrackDests = std::vector<int>(34);
		bool notePassEnabled{};
		bool pitchBendPassEnabled{};
		bool pgmChangePassEnabled{};
		bool chPressurePassEnabled{};
		bool polyPressurePassEnabled{};
		bool exclusivePassEnabled{};
		std::vector<char> saveBytes{};

	public:
		int getReceiveCh();
		bool isSustainPedalToDurationEnabled();
		bool isFilterEnabled();
		int getFilterType();
		bool isMultiRecEnabled();
		std::vector<int> getMultiRecTrackDests();
		bool isNotePassEnabled();
		bool isPitchBendPassEnabled();
		bool isPgmChangePassEnabled();
		bool isChPressurePassEnabled();
		bool isPolyPressurePassEnabled();
		bool isExclusivePassEnabled();

	public:
		std::vector<char> getBytes();

		MidiInput(const std::vector<char>& b);
		MidiInput();

	};
}
