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

namespace mpc::sampler
{
    class Program;
}

namespace mpc::performance
{
    class PerformanceManager final
        : public concurrency::AtomicStateExchange<
              PerformanceState, PerformanceStateView, PerformanceMessage>
    {
    public:
        PerformanceManager();
        ~PerformanceManager() override;

        void registerUpdateDrumProgram(DrumBusIndex, ProgramIndex);

        void registerPhysicalPadPress(
            PerformanceEventSource, lcdgui::ScreenId, sequencer::BusType,
            PhysicalPadIndex, Velocity, TrackIndex, controller::Bank,
            std::optional<ProgramIndex>, std::optional<NoteNumber>);

        void registerPhysicalPadAftertouch(
            PhysicalPadIndex, Pressure, PerformanceEventSource);

        void registerPhysicalPadRelease(
            PhysicalPadIndex, PerformanceEventSource);

        void registerProgramPadPress(
            PerformanceEventSource, std::optional<MidiChannel> midiInputChannel,
            lcdgui::ScreenId, TrackIndex, sequencer::BusType, ProgramPadIndex,
            Velocity, ProgramIndex, PhysicalPadIndex);

        void registerProgramPadAftertouch(PerformanceEventSource,
                                          ProgramPadIndex, ProgramIndex,
                                          Pressure);

        void registerProgramPadRelease(
            PerformanceEventSource, ProgramPadIndex, ProgramIndex);

        NoteOnEvent
        registerNoteOn(PerformanceEventSource,
                       std::optional<MidiChannel> midiInputChannel,
                       lcdgui::ScreenId, TrackIndex, sequencer::BusType,
                       NoteNumber, Velocity, std::optional<ProgramIndex>);

        void registerNoteAftertouch(
            PerformanceEventSource, NoteNumber, Pressure,
            std::optional<MidiChannel> midiInputChannel);

        void registerNoteOff(PerformanceEventSource, NoteNumber,
                             std::optional<MidiChannel> midiInputChannel);

        void clear();

    protected:
        void applyMessage(const PerformanceMessage &) noexcept override;

    private:
        static constexpr size_t CAPACITY = 8192;

        static void reserveState(PerformanceState &);
    };
} // namespace mpc::performance
