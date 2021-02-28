#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mpc::sequencer
{
class Sequence;
class Event;
}

namespace mpc::file::all
{
class BarList;
class Tracks;
}

namespace mpc::file::all
{
class AllSequence
{
    
public:
    static const int MAX_SYSEX_SIZE{ 256 };
    static const int EVENT_ID_OFFSET{ 4 };
    static const char POLY_PRESSURE_ID = 0xA0;
    static const char CONTROL_CHANGE_ID = 0xB0;
    static const char PGM_CHANGE_ID = 0xC0;
    static const char CH_PRESSURE_ID = 0xD0;
    static const char PITCH_BEND_ID = 0xE0;
    static const char SYS_EX_ID = 0xF0;
    static const char SYS_EX_TERMINATOR_ID = 0xF8;
    
private:
    static std::vector<char> TERMINATOR;
    
public:
    static const int MAX_EVENT_SEG_COUNT{ 50000 };
    static const int EVENT_SEG_LENGTH{ 8 };
    static const int NAME_OFFSET{ 0 };
    static const int LAST_EVENT_INDEX_OFFSET{ 16 };
    static const int SEQUENCE_INDEX_OFFSET{ 18 };
    static const int PADDING1_OFFSET{ 19 };
    
private:
    static std::vector<char> PADDING1;
    
public:
    static const int TEMPO_BYTE1_OFFSET{ 22 };
    static const int TEMPO_BYTE2_OFFSET{ 23 };
    static const int PADDING2_OFFSET{ 24 };
    
private:
    static std::vector<char> PADDING2;
    
public:
    static const int BAR_COUNT_BYTE1_OFFSET{ 26 };
    static const int BAR_COUNT_BYTE2_OFFSET{ 27 };
    static const int LAST_TICK_BYTE1_OFFSET{ 28 };
    static const int LAST_TICK_BYTE2_OFFSET{ 29 };
    static const int UNKNOWN32_BIT_INT_OFFSET{ 32 };
    static const int LOOP_FIRST_OFFSET{ 48 };
    static const int LOOP_LAST_OFFSET{ 50 };
    static const int LOOP_ENABLED_OFFSET{ 52 };
    static const int PADDING4_OFFSET{ 59 };
    
private:
    static std::vector<char> PADDING4;
    
public:
    static const int LAST_TICK_BYTE3_OFFSET{ 64 };
    static const int LAST_TICK_BYTE4_OFFSET{ 65 };
    static const int DEVICE_NAMES_OFFSET{ 120 };
    static const int TRACKS_OFFSET{ 384 };
    static const int TRACKS_LENGTH{ 1764 };
    static const int BAR_LIST_OFFSET{ 5379 };
    static const int BAR_LIST_LENGTH{ 3996 };
    static const int EVENTS_OFFSET{ 10240 };
    std::string name;
    int barCount;
    int loopFirst;
    int loopLast;
    bool loopLastEnd;
    bool loop;
    double tempo;
    std::vector<std::string> devNames = std::vector<std::string>(33);
    Tracks* tracks;
    BarList* barList;
    std::vector<std::shared_ptr<mpc::sequencer::Event>> allEvents;
    
private:
    std::vector<char> saveBytes;
    
private:
    static std::vector<std::shared_ptr<mpc::sequencer::Event>> readEvents(const std::vector<char>& seqBytes);
    static std::vector<std::vector<char>> readEventSegments(const std::vector<char>& seqBytes);
    double getTempoDouble(const std::vector<char>& bytePair);
    
public:
    static int getNumberOfEventSegmentsForThisSeq(const std::vector<char>& seqBytes);
    virtual int getEventAmount();
    
private:
    static int getSegmentCount(mpc::sequencer::Sequence* seq);
    void setUnknown32BitInt(mpc::sequencer::Sequence* seq);
    void setBarCount(int i);
    std::vector<char> createEventSegmentsChunk(mpc::sequencer::Sequence* seq);
    void setTempoDouble(double tempo);
    void setLastTick(mpc::sequencer::Sequence* seq);
    
public:
    std::vector<char>& getBytes();
    
    void applyToMpcSeq(std::shared_ptr<mpc::sequencer::Sequence> mpcSeq);
    
    AllSequence(const std::vector<char>& b);
    AllSequence(mpc::sequencer::Sequence* seq, int number);
    
};
}
