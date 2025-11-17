#pragma once

#include "IntTypes.hpp"
#include "sampler/VoiceOverlapMode.hpp"
#include "performance/PerformanceMessage.hpp"

#include <memory>
#include <functional>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc::sampler
{
    class Sampler;

    class NoteParameters
    {
    public:
        explicit NoteParameters(
            int index, const std::function<ProgramIndex()> &getProgramIndex,
            const std::function<performance::NoteParameters()> &getSnapshot,
            const std::function<void(performance::PerformanceMessage &&)>
                &dispatch);

        NoteParameters *clone(int newIndex) const;

        DrumNoteNumber getNumber() const;

        void setSoundIndex(int i) const;

        void setSoundGenMode(int i) const;

        void setVeloRangeLower(int i) const;

        int getVelocityRangeLower() const;

        void setOptionalNoteA(DrumNoteNumber) const;

        void setVeloRangeUpper(int i) const;

        int getVelocityRangeUpper() const;

        void setOptionalNoteB(DrumNoteNumber) const;

        void setVoiceOverlapMode(VoiceOverlapMode) const;

        void setMuteAssignA(DrumNoteNumber) const;

        void setMuteAssignB(DrumNoteNumber) const;

        void setTune(int i) const;

        void setAttack(int i) const;

        void setDecay(int i) const;

        void setDecayMode(int i) const;

        void setFilterFrequency(int i) const;

        void setFilterResonance(int i) const;

        void setFilterAttack(int i) const;

        void setFilterDecay(int i) const;

        void setFilterEnvelopeAmount(int i) const;

        void setVeloToLevel(int i) const;

        void setVelocityToAttack(int i) const;

        void setVelocityToStart(int i) const;

        void setVelocityToFilterFrequency(int i) const;

        void setSliderParameterNumber(int i) const;

        int getSliderParameterNumber() const;

        void setVelocityToPitch(int i) const;

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

        std::shared_ptr<engine::StereoMixer> getStereoMixer();

        std::shared_ptr<engine::IndivFxMixer> getIndivFxMixer();

    private:
        const int index;
        const std::function<ProgramIndex()> getProgramIndex;
        const std::function<performance::NoteParameters()> getSnapshot;
        const std::function<void(performance::PerformanceMessage &&)> dispatch;
        std::shared_ptr<engine::StereoMixer> stereoMixer;
        std::shared_ptr<engine::IndivFxMixer> indivFxMixer;
    };
} // namespace mpc::sampler
