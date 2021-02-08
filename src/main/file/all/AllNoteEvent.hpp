#pragma once

#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {
class AllNoteEvent
{
    
public:
    static const int DURATION_BYTE1_OFFSET{ 2 };
    static std::vector<int> DURATION_BYTE1_BIT_RANGE;
    static const int DURATION_BYTE2_OFFSET{ 3 };
    static std::vector<int> DURATION_BYTE2_BIT_RANGE;
    static const int TRACK_NUMBER_OFFSET{ 3 };
    static std::vector<int> TRACK_NUMBER_BIT_RANGE;
    static const int NOTE_NUMBER_OFFSET{ 4 };
    static const int DURATION_BYTE3_OFFSET{ 5 };
    static const int VELOCITY_OFFSET{ 6 };
    static std::vector<int> VELOCITY_BIT_RANGE;
    static const int VAR_TYPE_BYTE1_OFFSET{ 6 };
    static const int VAR_TYPE_BYTE1_BIT{ 7 };
    static const int VAR_TYPE_BYTE2_OFFSET{ 7 };
    static const int VAR_TYPE_BYTE2_BIT{ 7 };
    static const int VAR_VALUE_OFFSET{ 7 };
    static std::vector<int> VAR_VALUE_BIT_RANGE;
    
public:
    int note{};
    int tick{};
    int duration{};
    int track{};
    int velocity{};
    int variationType{};
    int variationValue{};
    std::vector<char> saveBytes{};
    
public:
    int getNote();
    int getTick();
    int getDuration();
    int getTrack();
    int getVelocity();
    int getVariationType();
    int getVariationValue();
    
private:
    int readDuration(std::vector<char> b);
    int readTrackNumber(std::vector<char> ba);
    int readVelocity(std::vector<char> ba);
    int readVariationValue(std::vector<char> ba);
    int readVariationType(std::vector<char> ba);
    
public:
    std::vector<char> setVelocity(std::vector<char> event, int v);
    std::vector<char> setTrackNumber(std::vector<char> event, int t);
    std::vector<char> setVariationValue(std::vector<char> event, int v);
    std::vector<char> setDuration(std::vector<char> event, int duration);
    std::vector<char> setVariationType(std::vector<char> event, int type);
    std::vector<char> getBytes();
    
    AllNoteEvent(const std::vector<char>& b);
    AllNoteEvent(mpc::sequencer::Event* event);
};
}
