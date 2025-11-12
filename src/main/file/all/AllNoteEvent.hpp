#pragma once

#include <memory>
#include <vector>
#include "sequencer/NoteEvent.hpp"

namespace mpc::file::all
{
    class AllEvent;
    class AllNoteOnEvent
    {
        static const int DURATION_BYTE1_OFFSET = 2;
        static std::vector<int> DURATION_BYTE1_BIT_RANGE;
        static const int DURATION_BYTE2_OFFSET = 3;
        static std::vector<int> DURATION_BYTE2_BIT_RANGE;
        static const int TRACK_NUMBER_OFFSET = 3;
        static std::vector<int> TRACK_NUMBER_BIT_RANGE;
        static const int NOTE_NUMBER_OFFSET = 4;
        static const int DURATION_BYTE3_OFFSET = 5;
        static const int VELOCITY_OFFSET = 6;
        static std::vector<int> VELOCITY_BIT_RANGE;
        static const int VAR_TYPE_BYTE1_OFFSET = 6;
        static const int VAR_TYPE_BYTE1_BIT = 7;
        static const int VAR_TYPE_BYTE2_OFFSET = 7;
        static const int VAR_TYPE_BYTE2_BIT = 7;
        static const int VAR_VALUE_OFFSET = 7;
        static std::vector<int> VAR_VALUE_BIT_RANGE;

        static int readDuration(const std::vector<char> &);
        static int readTrackNumber(const std::vector<char> &);
        static int readVelocity(const std::vector<char> &);
        static int readVariationValue(const std::vector<char> &);
        static sequencer::NoteOnEvent::VARIATION_TYPE
        readVariationType(const std::vector<char> &);

        static void writeVelocity(std::vector<char> &event, int);
        static void writeTrackNumber(std::vector<char> &event, int);
        static void writeVariationValue(std::vector<char> &event, int);
        static void writeDuration(std::vector<char> &event, int);
        static void writeVariationType(std::vector<char> &event, int);

        friend class AllEvent;

    public:
        static std::shared_ptr<sequencer::NoteOnEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const std::shared_ptr<sequencer::NoteOnEvent> &);
    };
} // namespace mpc::file::all
