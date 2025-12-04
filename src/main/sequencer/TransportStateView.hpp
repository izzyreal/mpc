#pragma once

#include "TransportState.hpp"

#include <cstdint>

namespace mpc::sequencer
{
    class TransportStateView
    {
    public:
        explicit TransportStateView(const TransportState &s) noexcept;

        double getPositionQuarterNotes() const;

        double getPlayStartPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

        bool isSequencerRunning() const;

        bool isCountEnabled() const;

        bool isRecording() const;

        bool isOverdubbing() const;

        bool isRecordingOrOverdubbing() const;

        bool isCountingIn() const;

        bool isMetronomeOnlyEnabled() const;

        Tick getMetronomeOnlyPositionTicks() const;

        Tick countInStartPosTicks() const;

        Tick countInEndPosTicks() const;

        int getPlayedSongStepRepetitionCount() const;

        double getMasterTempo() const;

        bool isTempoSourceSequenceEnabled() const;

        bool isShouldWaitForMidiClockLockEnabled() const;

    private:
        const TransportState state;
    };
} // namespace mpc::sequencer
