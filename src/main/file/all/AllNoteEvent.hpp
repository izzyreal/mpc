#pragma once

#include "performance/Sequence.hpp"

namespace mpc::file::all
{
    class AllEvent;
    class AllNoteOnEvent
    {
        static constexpr int DURATION_BYTE1_OFFSET = 2;
        static std::vector<int> DURATION_BYTE1_BIT_RANGE;
        static constexpr int DURATION_BYTE2_OFFSET = 3;
        static std::vector<int> DURATION_BYTE2_BIT_RANGE;
        static constexpr int TRACK_NUMBER_OFFSET = 3;
        static std::vector<int> TRACK_NUMBER_BIT_RANGE;
        static constexpr int NOTE_NUMBER_OFFSET = 4;
        static constexpr int DURATION_BYTE3_OFFSET = 5;
        static constexpr int VELOCITY_OFFSET = 6;
        static std::vector<int> VELOCITY_BIT_RANGE;
        static constexpr int VAR_TYPE_BYTE1_OFFSET = 6;
        static constexpr int VAR_TYPE_BYTE1_BIT = 7;
        static constexpr int VAR_TYPE_BYTE2_OFFSET = 7;
        static constexpr int VAR_TYPE_BYTE2_BIT = 7;
        static constexpr int VAR_VALUE_OFFSET = 7;
        static std::vector<int> VAR_VALUE_BIT_RANGE;

        static int readDuration(const std::vector<char> &);
        static int readTrackNumber(const std::vector<char> &);
        static int readVelocity(const std::vector<char> &);
        static int readVariationValue(const std::vector<char> &);
        static NoteVariationType
        readVariationType(const std::vector<char> &);

        static void writeVelocity(std::vector<char> &event, int);
        static void writeTrackNumber(std::vector<char> &event, int);
        static void writeVariationValue(std::vector<char> &event, int);
        static void writeDuration(std::vector<char> &event, int);
        static void writeVariationType(std::vector<char> &event, int);

        friend class AllEvent;

    public:
        static performance::Event
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const performance::Event &);
    };
} // namespace mpc::file::all
