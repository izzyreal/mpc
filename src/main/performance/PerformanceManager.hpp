#pragma once

#include "performance/PerformanceState.hpp"
#include "concurrency/AtomicStateExchange.hpp"
#include "IntTypes.hpp"
#include "performance/PerformanceEventSource.hpp"
#include "performance/EventTypes.hpp"
#include "performance/PerformanceMessage.hpp"
#include "performance/PerformanceStateView.hpp"
#include "sequencer/BusType.hpp"

#include <optional>
#include <functional>

namespace mpc::eventregistry
{
    class PerformanceManager final
        : public concurrency::AtomicStateExchange<PerformanceState, PerformanceStateView,
                                                  PerformanceMessage>
    {
    public:
        PerformanceManager();

        void registerPhysicalPadPress(
            PerformanceEventSource, lcdgui::ScreenId, sequencer::BusType, PhysicalPadIndex,
            Velocity, TrackIndex, controller::Bank, std::optional<ProgramIndex>,
            std::optional<NoteNumber>,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadAftertouch(
            PhysicalPadIndex, Pressure, PerformanceEventSource,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadRelease(
            PhysicalPadIndex, PerformanceEventSource,
            const std::function<void(void *)> &action) const;

        void registerProgramPadPress(
            PerformanceEventSource, std::optional<MidiChannel> midiInputChannel,
            lcdgui::ScreenId, TrackIndex, sequencer::BusType, ProgramPadIndex,
            Velocity, ProgramIndex, PhysicalPadIndex) const;

        void registerProgramPadAftertouch(PerformanceEventSource, ProgramPadIndex, ProgramIndex,
                                          Pressure) const;

        void registerProgramPadRelease(
            PerformanceEventSource, ProgramPadIndex, ProgramIndex,
            const std::function<void(void *)> &action) const;

        NoteOnEvent
        registerNoteOn(PerformanceEventSource, std::optional<MidiChannel> midiInputChannel,
                       lcdgui::ScreenId, TrackIndex, sequencer::BusType,
                       NoteNumber, Velocity, std::optional<ProgramIndex>,
                       const std::function<void(void *)> &action) const;

        void registerNoteAftertouch(
            PerformanceEventSource, NoteNumber, Pressure,
            std::optional<MidiChannel> midiInputChannel) const;

        void registerNoteOff(PerformanceEventSource, NoteNumber,
                             std::optional<MidiChannel> midiInputChannel,
                             const std::function<void(void *)> &action) const;

        void clear() const;

    protected:
        void applyMessage(const PerformanceMessage &) noexcept override;

    private:
        static constexpr size_t CAPACITY = 8192;
        void reserveState(PerformanceState &) const;
    };
} // namespace mpc::eventregistry
