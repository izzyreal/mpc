#pragma once

#include "sampler/VoiceOverlapMode.hpp"

#include <vector>

namespace mpc::sampler
{
    class NoteParameters;
}

namespace mpc::file::aps
{
    class ApsNoteParameters
    {

    public:
        int soundIndex;
        int soundGenerationMode;
        int velocityRangeLower;
        int alsoPlay1;
        int velocityRangeUpper;
        int alsoPlay2;
        sampler::VoiceOverlapMode voiceOverlapMode;
        int mute1;
        int mute2;
        int tune;
        int attack;
        int decay;
        int decayMode;
        int cutoffFrequency;
        int resonance;
        int filterAttack;
        int filterDecay;
        int filterEnvelopeAmount;
        int velocityToLevel;
        int velocityToAttack;
        int velocityToStart;
        int velocityToFilterFrequency;
        int sliderParameter;
        int velocityToPitch;
        std::vector<char> saveBytes = std::vector<char>(26);

    public:
        int getSoundIndex();
        sampler::VoiceOverlapMode getVoiceOverlapMode();
        int getTune();
        int getDecayMode();
        int getSoundGenerationMode();
        int getVelocityRangeLower();
        int getAlsoPlay1();
        int getVelocityRangeUpper();
        int getAlsoPlay2();
        int getMute1();
        int getMute2();
        int getAttack();
        int getDecay();
        int getCutoffFrequency();
        int getResonance();
        int getVelocityToFilterAttack();
        int getVelocityToFilterDecay();
        int getVelocityToFilterAmount();
        int getVelocityToLevel();
        int getVelocityToAttack();
        int getVelocityToStart();
        int getVelocityToFilterFrequency();
        int getSliderParameter();
        int getVelocityToPitch();
        std::vector<char> getBytes();

        ApsNoteParameters(const std::vector<char> &loadBytes);
        ApsNoteParameters(mpc::sampler::NoteParameters *np);
    };
} // namespace mpc::file::aps
