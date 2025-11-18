#pragma once

#include <functional>

#include "performance/Drum.hpp"
#include "performance/PerformanceMessage.hpp"

namespace mpc::engine
{
    class IndivFxMixer
    {
    public:
        IndivFxMixer(
            const std::function<performance::IndivFxMixer()> &getSnapshot,
            const std::function<DrumBusIndex()> &getDrumIndex,
            const std::function<ProgramIndex()> &getProgramIndex,
            const std::function<DrumNoteNumber()> &getDrumNoteNumber,
            const std::function<void(performance::PerformanceMessage &&)>
                &dispatch);

        void setFollowStereo(bool b) const;

        bool isFollowingStereo() const;

        DrumMixerIndividualOutput getOutput() const;

        void setOutput(DrumMixerIndividualOutput) const;

        void setVolumeIndividualOut(DrumMixerLevel) const;

        DrumMixerLevel getVolumeIndividualOut() const;

        void setFxPath(DrumMixerIndividualFxPath) const;

        DrumMixerIndividualFxPath getFxPath() const;

        void setFxSendLevel(DrumMixerLevel) const;

        DrumMixerLevel getFxSendLevel() const;

    private:
        const std::function<performance::IndivFxMixer()> getSnapshot;
        const std::function<DrumBusIndex()> getDrumIndex;
        const std::function<ProgramIndex()> getProgramIndex;
        const std::function<DrumNoteNumber()> getDrumNoteNumber;
        std::function<void(performance::PerformanceMessage &&)> dispatch;
    };
} // namespace mpc::engine
