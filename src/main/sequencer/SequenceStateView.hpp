#pragma once

#include "IntTypes.hpp"
#include "TimeSignature.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class TrackStateView;
    struct SequenceState;
    class SequenceStateView
    {
    public:
        explicit SequenceStateView(
            const SequenceState &s) noexcept;

        std::shared_ptr<TrackStateView> getTrack(int trackIndex) const;
        Tick getBarLength(int barIndex) const;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> getBarLengths() const;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> getTimeSignatures() const;

        TimeSignature getTimeSignature(int barIndex) const;

        BarIndex getLastBarIndex() const;

        int getBarCount() const;

        Tick getFirstTickOfBar(BarIndex) const;

        Tick getLastTick() const;

        double getInitialTempo() const;

        Tick getLoopStartTick() const;

        Tick getLoopEndTick() const;

        BarIndex getFirstLoopBarIndex() const;

        BarIndex getLastLoopBarIndex() const;

        bool isLoopEnabled() const;

        bool isUsed() const;

        bool isTempoChangeEnabled() const;

    private:
        const SequenceState &state;
    };
} // namespace mpc::sequencer
