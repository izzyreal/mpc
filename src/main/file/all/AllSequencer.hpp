#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::file::all
{
class AllSequencer
{
public:
    static const int LENGTH = 11;
    
private:
    static std::vector<char> TEMPLATE;
    
public:
    static const int SEQ_OFFSET = 0;
    static const int TR_OFFSET = 2;
    static const int MASTER_TEMPO_OFFSET = 4;
    static const int TEMPO_SOURCE_IS_SEQUENCE_OFFSET = 6;
    static const int TC_OFFSET = 7;
    static const int TIME_DISPLAY_STYLE_OFFSET = 8;
    static const int SECOND_SEQ_ENABLED_OFFSET = 9;
    static const int SECOND_SEQ_INDEX_OFFSET = 10;
    
    int sequence;
    int track;
    double masterTempo;
    bool tempoSourceIsSequence;
    int tc;
    int timeDisplayStyle;
    bool secondSeqEnabled;
    int secondSeqIndex;
    std::vector<char> saveBytes;
    
public:
    std::vector<char>& getBytes();
    
    AllSequencer(const std::vector<char>& loadBytes);
    AllSequencer(mpc::Mpc&);
    
};
}
