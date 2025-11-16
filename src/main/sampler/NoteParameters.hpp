#pragma once

#include "IntTypes.hpp"
#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include "sampler/VoiceOverlapMode.hpp"
#include "performance/PerformanceMessage.hpp"

#include <memory>
#include <functional>

namespace mpc::sampler
{
    class Sampler;

    class NoteParameters
    {
    public:
        explicit NoteParameters(int index, std::function<void(performance::PerformanceMessage&)> dispatch);

        NoteParameters *clone(int newIndex) const;
        DrumNoteNumber getNumber() const;

        void setSoundIndex(int i);
        void setSoundGenMode(int i);
        void setVeloRangeLower(int i);
        int getVelocityRangeLower() const;
        void setOptNoteA(DrumNoteNumber);
        void setVeloRangeUpper(int i);
        int getVelocityRangeUpper() const;
        void setOptionalNoteB(DrumNoteNumber);
        void setVoiceOverlapMode(VoiceOverlapMode);
        void setMuteAssignA(DrumNoteNumber);
        void setMuteAssignB(DrumNoteNumber);
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
        int getSliderParameterNumber() const;
        void setVelocityToPitch(int i);
        int getVelocityToPitch() const;
        int getSoundIndex() const;
        int getSoundGenerationMode() const;
        DrumNoteNumber getOptionalNoteA() const;
        DrumNoteNumber getOptionalNoteB() const;
        DrumNoteNumber getMuteAssignA() const;
        DrumNoteNumber getMuteAssignB() const;
        int getTune() const;
        int getVelocityToStart() const;
        int getAttack() const;
        int getDecay() const;
        int getVelocityToAttack() const;
        int getDecayMode() const;
        int getVeloToLevel() const;
        int getFilterFrequency() const;
        int getVelocityToFilterFrequency() const;
        int getFilterAttack() const;
        int getFilterDecay() const;
        int getFilterResonance() const;
        int getFilterEnvelopeAmount() const;
        VoiceOverlapMode getVoiceOverlapMode() const;

        std::shared_ptr<engine::StereoMixer> getStereoMixerChannel();
        std::shared_ptr<engine::IndivFxMixer> getIndivFxMixerChannel();

    private:
        const int index;
        const std::function<void(performance::PerformanceMessage&)> dispatch;
        std::shared_ptr<engine::StereoMixer> stereoMixerChannel =
            std::make_shared<engine::StereoMixer>();
        std::shared_ptr<engine::IndivFxMixer> indivFxMixerChannel =
            std::make_shared<engine::IndivFxMixer>();
        int soundIndex = -1;
        int soundGenerationMode = 0;
        int velocityRangeLower = 0;
        DrumNoteNumber optionalNoteA{NoDrumNoteAssigned};
        int velocityRangeUpper = 0;
        DrumNoteNumber optionalNoteB{NoDrumNoteAssigned};
        VoiceOverlapMode voiceOverlapMode = VoiceOverlapMode::POLY;
        DrumNoteNumber muteAssignA{NoDrumNoteAssigned};
        DrumNoteNumber muteAssignB{NoDrumNoteAssigned};
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
    };
} // namespace mpc::sampler
