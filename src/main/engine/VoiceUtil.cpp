#include "VoiceUtil.hpp"

#include <cmath>

using namespace mpc::engine;

float VoiceUtil::getInverseNyquist(const int sampleRate)
{
    return 2.f / sampleRate;
}

std::vector<float>& VoiceUtil::freqTable()
{
    static std::vector<float> res;

    if (res.empty())
    {
        for (auto i = 0; i < 140; i++)
        {
            res.push_back(midiFreqImpl(i));
        }
    }

    return res;
}

float VoiceUtil::midiFreq(const float pitch)
{
    if (pitch < 0)
    {
        return freqTable()[0];
    }

    if (pitch >= (int)(freqTable().size()) - 1)
    {
        return freqTable()[(int)(freqTable().size()) - 2];
    }

    const auto idx = (int)(pitch);
    const auto frac = pitch - idx;

    return freqTable()[idx] * (1 - frac) + freqTable()[idx + 1] * frac;
}

float VoiceUtil::midiFreqImpl(const int pitch)
{
    return (float)(440.0 * pow(2.0, ((double)(pitch) - 69.0) / 12.0));
}

