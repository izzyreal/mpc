#pragma once

#include <vector>

namespace mpc::engine {

    class VoiceUtil 
    {
    public:
        static float getInverseNyquist(const int sampleRate);
        static float midiFreq(const float pitch);
        static float midiFreqImpl(const int pitch);
        static std::vector<float>& freqTable();

    };
}

