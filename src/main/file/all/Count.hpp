#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::file::all {

class Count
{
    
public:
    static const int ENABLED_OFFSET = 0;
    static const int COUNT_IN_MODE_OFFSET = 1;
    static const int CLICK_VOLUME_OFFSET = 2;
    static const int RATE_OFFSET = 3;
    static const int ENABLED_IN_PLAY_OFFSET = 4;
    static const int ENABLED_IN_REC_OFFSET = 5;
    static const int CLICK_OUTPUT_OFFSET = 6;
    static const int WAIT_FOR_KEY_ENABLED_OFFSET = 7;
    static const int SOUND_OFFSET = 8;
    static const int ACCENT_VELO_OFFSET = 9;
    static const int NORMAL_VELO_OFFSET = 10;
    bool enabled;
    int countInMode;
    int clickVolume;
    int rate;
    bool enabledInPlay;
    bool enabledInRec;
    int clickOutput;
    bool waitForKeyEnabled;
    int sound;
    int accentVelo;
    int normalVelo;
    std::vector<char> saveBytes;
    
public:
    bool isEnabled();
    int getCountInMode();
    int getClickVolume();
    int getRate();
    bool isEnabledInPlay();
    bool isEnabledInRec();
    int getClickOutput();
    bool isWaitForKeyEnabled();
    int getSound();
    int getAccentVelo();
    int getNormalVelo();
    
public:
    std::vector<char>& getBytes();
    
    Count(const std::vector<char>& b);
    Count(mpc::Mpc& mpc);

};
}
