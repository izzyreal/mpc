#pragma once

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "sampler/VoiceOverlapMode.hpp"

#include <memory>

namespace mpc::sampler
{
    class Sampler;

    class NoteParameters
    {
    public:
        int getSoundIndex();
        int getSoundGenerationMode();
        int getOptionalNoteA();
        int getOptionalNoteB();
        int getMuteAssignA();
        int getMuteAssignB();
        int getTune();
        int getVelocityToStart();
        int getAttack();
        int getDecay();
        int getVelocityToAttack();
        int getDecayMode();
        int getVeloToLevel();
        int getFilterFrequency();
        int getVelocityToFilterFrequency();
        int getFilterAttack();
        int getFilterDecay();
        int getFilterResonance();
        int getFilterEnvelopeAmount();
        VoiceOverlapMode getVoiceOverlapMode();

        std::shared_ptr<mpc::engine::StereoMixer> getStereoMixerChannel();
        std::shared_ptr<mpc::engine::IndivFxMixer> getIndivFxMixerChannel();

    private:
        std::shared_ptr<mpc::engine::StereoMixer> stereoMixerChannel = std::make_shared<mpc::engine::StereoMixer>();
        std::shared_ptr<mpc::engine::IndivFxMixer> indivFxMixerChannel = std::make_shared<mpc::engine::IndivFxMixer>();
        int soundIndex = -1;
        int soundGenerationMode = 0;
        int velocityRangeLower = 0;
        int optionalNoteA = 0;
        int velocityRangeUpper = 0;
        int optionalNoteB = 0;
        VoiceOverlapMode voiceOverlapMode = VoiceOverlapMode::POLY;
        int muteAssignA = 0;
        int muteAssignB = 0;
        int tune = 0;
        int attack = 0;
        int decay = 0;
        int decayMode = 0;
        int filterFrequency = 0;
        int filterResonance = 0;
        int filterAttack = 0;
        int filterDecay = 0;
        int filterEnvelopeAmount = 0;
        int velocityToLevel = 0;
        int velocityToAttack = 0;
        int velocityToStart = 0;
        int velocityToFilterFrequency = 0;
        int sliderParameterNumber = 0;
        int velocityToPitch = 0;
        int index = 0;

    public:
        void setSoundNumberNoLimit(int i);
        void setSoundIndex(int i);
        void setSoundGenMode(int i);
        void setVeloRangeLower(int i);
        int getVelocityRangeLower();
        void setOptNoteA(int i);
        void setVeloRangeUpper(int i);
        int getVelocityRangeUpper();
        void setOptionalNoteB(int i);
        void setVoiceOverlapMode(const VoiceOverlapMode);
        void setMuteAssignA(int i);
        void setMuteAssignB(int i);
        void setTune(int i);
        void setAttack(int i);
        void setDecay(int i);
        void setDecayMode(int i);
        void setFilterFrequency(int i);
        void setFilterResonance(int i);
        void setFilterAttack(int i);
        void setFilterDecay(int i);
        void setFilterEnvelopeAmount(int i);
        void setVeloToLevel(int i);
        void setVelocityToAttack(int i);
        void setVelocityToStart(int i);
        void setVelocityToFilterFrequency(int i);
        void setSliderParameterNumber(int i);
        int getSliderParameterNumber();
        void setVelocityToPitch(int i);
        int getVelocityToPitch();
        NoteParameters *clone(const int newIndex);
        int getNumber();

        NoteParameters(int index);
    };
} // namespace mpc::sampler
