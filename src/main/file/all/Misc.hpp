#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::file::all
{
class Misc
{
public:
    static const int LENGTH = 128;

    std::vector<std::pair<int, int>> getSwitches();
    int getTapAvg();
    bool isInReceiveMMCEnabled();
    bool isAutoStepIncEnabled();
    bool isDurationOfRecNotesTc();
    int getDurationTcPercentage();
    bool isPgmChToSeqEnabled();
    
    std::vector<char> getBytes();
    
    Misc(const std::vector<char>& b);
    Misc(mpc::Mpc&);
    
private:
    static const int TAP_AVG_OFFSET = 33;
    static const int MIDI_SYNC_IN_RECEIVE_MMC_OFFSET = 34;
    static const int MIDI_SWITCH_OFFSET = 35;
    static const int AUTO_STEP_INCREMENT_OFFSET = 46;
    static const int DURATION_OF_REC_NOTES_OFFSET = 47;
    static const int DURATION_TC_PERCENTAGE_OFFSET = 48;
    static const int MIDI_PGM_CHANGE_TO_SEQ_OFFSET = 49;

    int tapAvg;
    bool inReceiveMMCEnabled;
    bool autoStepInc;
    bool durationOfRecNotesTcEnabled;
    int durationTcPercentage;
    bool pgmChToSeqEnabled;
    std::vector<char> saveBytes;
    std::vector<std::pair<int, int>> switches = std::vector<std::pair<int, int>>(4);
    
};
}
