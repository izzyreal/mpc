#pragma once

#include "performance/Drum.hpp"
#include "performance/PerformanceMessage.hpp"

#include <functional>

namespace mpc::engine
{
    class StereoMixer
    {
    public:
        StereoMixer(
            const std::function<performance::StereoMixer()> &getSnapshot,
            const std::function<void(performance::PerformanceMessage &)>
                &dispatch);

        void setPanning(DrumMixerPanning) const;

        DrumMixerPanning getPanning() const;

        void setLevel(DrumMixerLevel) const;

        DrumMixerLevel getLevel() const;

    private:
        const std::function<performance::StereoMixer()> getSnapshot;
        std::function<void(performance::PerformanceMessage &)> dispatch;
    };
} // namespace mpc::engine
