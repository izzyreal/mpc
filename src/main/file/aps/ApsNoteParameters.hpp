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

        int getSoundIndex() const;
        sampler::VoiceOverlapMode getVoiceOverlapMode() const;
        int getTune() const;
        int getDecayMode() const;
        int getSoundGenerationMode() const;
        int getVelocityRangeLower() const;
        int getAlsoPlay1() const;
        int getVelocityRangeUpper() const;
        int getAlsoPlay2() const;
        int getMute1() const;
        int getMute2() const;
        int getAttack() const;
        int getDecay() const;
        int getCutoffFrequency() const;
        int getResonance() const;
        int getVelocityToFilterAttack() const;
        int getVelocityToFilterDecay() const;
        int getVelocityToFilterAmount() const;
        int getVelocityToLevel() const;
        int getVelocityToAttack() const;
        int getVelocityToStart() const;
        int getVelocityToFilterFrequency() const;
        int getSliderParameter() const;
        int getVelocityToPitch() const;
        std::vector<char> getBytes();

        ApsNoteParameters(const std::vector<char> &loadBytes);
        ApsNoteParameters(sampler::NoteParameters *np);
    };
} // namespace mpc::file::aps
