#pragma once

#include <memory>
#include <string>
#include <vector>

#include "sequencer/Sequence.hpp"
#include "sequencer/EventData.hpp"

namespace mpc::sequencer
{
    class Sequence;
    class EventRef;
} // namespace mpc::sequencer

namespace mpc::file::all
{
    class BarList;
    class Tracks;
} // namespace mpc::file::all

namespace mpc::file::all
{
    class AllSequence final
    {
    public:
        static constexpr int MAX_SYSEX_SIZE{256};
        static constexpr int EVENT_ID_OFFSET{4};
        static constexpr char POLY_PRESSURE_ID = 0xA0;
        static constexpr char CONTROL_CHANGE_ID = 0xB0;
        static constexpr char PGM_CHANGE_ID = 0xC0;
        static constexpr char CH_PRESSURE_ID = 0xD0;
        static constexpr char PITCH_BEND_ID = 0xE0;
        static constexpr char SYS_EX_ID = 0xF0;
        static constexpr char SYS_EX_TERMINATOR_ID = 0xF8;

    private:
        static std::vector<char> TERMINATOR;

    public:
        static constexpr int MAX_EVENT_SEG_COUNT{50000};
        static constexpr int EVENT_SEG_LENGTH{8};
        static constexpr int NAME_OFFSET{0};
        static constexpr int LAST_EVENT_INDEX_OFFSET{16};
        static constexpr int SEQUENCE_INDEX_OFFSET{18};
        static constexpr int PADDING1_OFFSET{19};

    private:
        static std::vector<char> PADDING1;

    public:
        static constexpr int TEMPO_BYTE1_OFFSET{22};
        static constexpr int TEMPO_BYTE2_OFFSET{23};
        static constexpr int PADDING2_OFFSET{24};

    private:
        static std::vector<char> PADDING2;

    public:
        static constexpr int BAR_COUNT_BYTE1_OFFSET{26};
        static constexpr int BAR_COUNT_BYTE2_OFFSET{27};
        static constexpr int LAST_TICK_BYTE1_OFFSET{28};
        static constexpr int LAST_TICK_BYTE2_OFFSET{29};
        static constexpr int UNKNOWN32_BIT_INT_OFFSET{32};
        static constexpr int LOOP_FIRST_OFFSET{48};
        static constexpr int LOOP_LAST_OFFSET{50};
        static constexpr int LOOP_ENABLED_OFFSET{52};
        static constexpr int START_TIME_OFFSET{53};
        static constexpr int PADDING4_OFFSET{59};

    private:
        static std::vector<char> PADDING4;

    public:
        static constexpr int LAST_TICK_BYTE3_OFFSET{64};
        static constexpr int LAST_TICK_BYTE4_OFFSET{65};
        static constexpr int DEVICE_NAMES_OFFSET{120};
        static constexpr int TRACKS_OFFSET{384};
        static constexpr int TRACKS_LENGTH{1764};
        static constexpr int BAR_LIST_OFFSET{5379};
        static constexpr int BAR_LIST_LENGTH{3996};
        static constexpr int EVENTS_OFFSET{10240};
        std::string name;
        int barCount;
        int loopFirst;
        int loopLast;
        bool loopLastEnd;
        bool loop;
        sequencer::Sequence::StartTime startTime{0, 0, 0, 0, 0};

        double tempo;
        std::vector<std::string> devNames = std::vector<std::string>(33);
        Tracks *tracks = nullptr;
        BarList *barList = nullptr;
        std::vector<sequencer::EventData> allEvents;

    private:
        std::vector<char> saveBytes;

        static std::vector<sequencer::EventData>
        readEvents(const std::vector<char> &seqBytes);
        static std::vector<std::vector<char>>
        readEventSegments(const std::vector<char> &seqBytes);
        static double getTempoDouble(const std::vector<char> &bytePair);

    public:
        static int
        getNumberOfEventSegmentsForThisSeq(const std::vector<char> &seqBytes);
        int getEventAmount() const;

    private:
        static int getSegmentCount(sequencer::Sequence *seq);
        void setUnknown32BitInt(const sequencer::Sequence *seq);
        void setBarCount(int i);
        static std::vector<char>
        createEventSegmentsChunk(sequencer::Sequence *seq);
        void setTempoDouble(double tempoForSaveBytes);
        void setLastTick(const sequencer::Sequence *seq);

    public:
        std::vector<char> &getBytes();

        void
        applyToMpcSeq(const std::shared_ptr<sequencer::Sequence> &mpcSeq,
                           const sequencer::SequencerStateManager *) const;

        explicit AllSequence(const std::vector<char> &bytes);
        AllSequence(sequencer::Sequence *seq, int number);
        ~AllSequence();
    };
} // namespace mpc::file::all
