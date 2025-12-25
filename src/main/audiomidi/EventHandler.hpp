#pragma once

#include "IntTypes.hpp"
#include "Observer.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/EventData.hpp"

#include <memory>
#include <optional>
#include <atomic>
#include <cstdint>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class DrumBus;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler final : public Observable
    {
    public:
        explicit EventHandler(Mpc &mpc);
        ~EventHandler() override;

    private:
        std::atomic<uint64_t> noteEventId = 1;

    public:
        void handleFinalizedEvent(const sequencer::EventData &,
                                  TrackIndex trackIndex, int trackVelocityRatio,
                                  sequencer::BusType, int trackDeviceIndex);

        void
        handleUnfinalizedNoteOn(const sequencer::EventData &,
                                std::optional<int> trackDevice,
                                std::optional<sequencer::BusType> drumBusType);

        void handleNoteOffFromUnfinalizedNoteOn(NoteNumber,
                                                std::optional<int> trackDevice,
                                                std::optional<DrumBusIndex>);

        void handleNoteEventMidiOut(const sequencer::EventData &,
                                    int trackDevice,
                                    std::optional<int> trackVelocityRatio,
                                    std::optional<int> transposeAmount,
                                    int sampleNumber);

    private:
        void handleFinalizedDrumNoteOnEvent(
            const sequencer::EventData &,
            const std::shared_ptr<sequencer::DrumBus> &, TrackIndex trackIndex,
            int trackVelocityRatio);

        Mpc &mpc;
    };
} // namespace mpc::audiomidi
