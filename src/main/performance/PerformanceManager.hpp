#pragma once

#include "performance/PerformanceState.hpp"
#include "concurrency/AtomicStateExchange.hpp"
#include "IntTypes.hpp"
#include "performance/PerformanceEventSource.hpp"
#include "performance/EventTypes.hpp"
#include "performance/PerformanceMessage.hpp"
#include "performance/PerformanceStateView.hpp"
#include "sequencer/BusType.hpp"

#include "utils/SmallFn.hpp"

#include <optional>

namespace mpc::sampler
{
    class Program;
}

namespace mpc::performance
{
    enum class UiCallbackPadEventType
    {
        Press,
        Release,
        Aftertouch
    };

    using PhysicalPadEventUiCallback =
        utils::SmallFn<8, void(PhysicalPadIndex, VelocityOrPressure,
                               UiCallbackPadEventType)>;

    using ProgramPadEventUiCallback =
        utils::SmallFn<8, void(ProgramPadIndex, VelocityOrPressure,
                               UiCallbackPadEventType)>;

    class PerformanceManager final
        : public concurrency::AtomicStateExchange<PerformanceState,
                                                  PerformanceStateView,
                                                  PerformanceMessage, 3, 8192>
    {
    public:
        explicit PerformanceManager(const utils::PostToUiThreadFn &);
        ~PerformanceManager() override;

        void registerUpdateDrumProgram(DrumBusIndex, ProgramIndex);

        void registerPhysicalPadPress(PerformanceEventSource, lcdgui::ScreenId,
                                      sequencer::BusType, PhysicalPadIndex,
                                      Velocity, TrackIndex,
                                      std::optional<ProgramIndex>,
                                      std::optional<DrumNoteNumber>);

        void registerPhysicalPadAftertouch(PhysicalPadIndex, Pressure,
                                           PerformanceEventSource);

        void registerPhysicalPadRelease(PhysicalPadIndex);

        void registerProgramPadPress(
            PerformanceEventSource, std::optional<MidiChannel> midiInputChannel,
            lcdgui::ScreenId, TrackIndex, sequencer::BusType, ProgramPadIndex,
            Velocity, ProgramIndex, PhysicalPadIndex);

        void registerProgramPadAftertouch(PerformanceEventSource,
                                          ProgramPadIndex, ProgramIndex,
                                          Pressure);

        void registerProgramPadRelease(PerformanceEventSource, ProgramPadIndex,
                                       ProgramIndex);

        NoteOnEvent registerNoteOn(PerformanceEventSource,
                                   PhysicalPadIndex,
                                   std::optional<MidiChannel> midiInputChannel,
                                   lcdgui::ScreenId, TrackIndex,
                                   sequencer::BusType, NoteNumber, Velocity,
                                   std::optional<ProgramIndex>);

        void
        registerNoteAftertouch(PerformanceEventSource, NoteNumber, Pressure,
                               std::optional<MidiChannel> midiInputChannel);

        void registerNoteOff(PerformanceEventSource, NoteNumber,
                             std::optional<MidiChannel> midiInputChannel);

        void clear();

        PhysicalPadEventUiCallback physicalPadEventUiCallback = {};
        ProgramPadEventUiCallback programPadEventUiCallback = {};

        SoundTable pgmFileSoundTable;
        SoundTable convertedSoundTable;

    protected:
        void applyMessage(const PerformanceMessage &) noexcept override;

    private:
        static constexpr size_t CAPACITY = 8192;

        const utils::PostToUiThreadFn &postToUiThread;

        static void reserveState(PerformanceState &);
    };
} // namespace mpc::performance
