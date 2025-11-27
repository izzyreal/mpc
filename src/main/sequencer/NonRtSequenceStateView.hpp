#pragma once

#include "IntTypes.hpp"
#include "TimeSignature.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class NonRtTrackStateView;
    struct NonRtSequenceState;
    class NonRtSequenceStateView
    {
    public:
        explicit NonRtSequenceStateView(
            const NonRtSequenceState &s) noexcept;

        std::shared_ptr<NonRtTrackStateView> getTrack(int trackIndex) const;
        Tick getBarLength(int barIndex) const;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> getBarLengths() const;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> getTimeSignatures() const;

        TimeSignature getTimeSignature(int barIndex) const;

        BarIndex getLastBarIndex() const;

        int getBarCount() const;

        Tick getFirstTickOfBar(BarIndex) const;

        Tick getLastTick() const;

    private:
        const NonRtSequenceState &state;
    };
} // namespace mpc::sequencer
