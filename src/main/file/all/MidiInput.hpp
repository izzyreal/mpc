#pragma once

#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::file::all
{

    class MidiInput
    {

    public:
        static const int LENGTH{62};
        static const int SOFT_THRU_MODE_OFFSET = 0;
        static const int RECEIVE_CH_OFFSET{1};
        static const int SUSTAIN_PEDAL_TO_DURATION_OFFSET{2};
        static const int FILTER_ENABLED_OFFSET{3};
        static const int FILTER_TYPE_OFFSET{4};
        static const int MULTI_REC_ENABLED_OFFSET{5};
        static const int MULTI_REC_TRACK_DESTS_OFFSET{6};
        static const int MULTI_REC_TRACK_DESTS_LENGTH{32};
        static const int NOTE_PASS_ENABLED_OFFSET{40};
        static const int PITCH_BEND_PASS_ENABLED_OFFSET{41};
        static const int PGM_CHANGE_PASS_ENABLED_OFFSET{42};
        static const int CH_PRESSURE_PASS_ENABLED_OFFSET{43};
        static const int POLY_PRESSURE_PASS_ENABLED_OFFSET{44};
        static const int EXCLUSIVE_PASS_ENABLED_OFFSET{45};
        static const int CC_PASS_ENABLED_OFFSET{46};

        int softThruMode;
        int receiveCh;
        bool sustainPedalToDuration;
        bool filterEnabled;
        int filterType;
        bool multiRecEnabled;
        std::vector<int> multiRecTrackDests = std::vector<int>(34);
        bool notePassEnabled;
        bool pitchBendPassEnabled;
        bool pgmChangePassEnabled;
        bool chPressurePassEnabled;
        bool polyPressurePassEnabled;
        bool exclusivePassEnabled;
        std::vector<bool> ccPassEnabled = std::vector<bool>(128);
        std::vector<char> saveBytes;

        int getSoftThruMode() const;
        int getReceiveCh() const;
        bool isSustainPedalToDurationEnabled() const;
        bool isFilterEnabled() const;
        int getFilterType() const;
        bool isMultiRecEnabled() const;
        std::vector<int> getMultiRecTrackDests();
        bool isNotePassEnabled() const;
        bool isPitchBendPassEnabled() const;
        bool isPgmChangePassEnabled() const;
        bool isChPressurePassEnabled() const;
        bool isPolyPressurePassEnabled() const;
        bool isExclusivePassEnabled() const;
        std::vector<bool> getCcPassEnabled();

        std::vector<char> &getBytes();

        MidiInput(const std::vector<char> &b);
        MidiInput(Mpc &mpc);
    };
} // namespace mpc::file::all
