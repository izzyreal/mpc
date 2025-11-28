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

    private:
        const TransportState state;
    };
} // namespace mpc::sequencer
