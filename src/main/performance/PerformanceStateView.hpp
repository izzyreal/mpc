#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <optional>

namespace mpc::performance
{
    struct PerformanceState;

    class PerformanceStateView
    {
    public:
        explicit PerformanceStateView(const PerformanceState *) noexcept;

        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;

        bool isProgramPadPressed(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex, ProgramIndex) const;

        std::optional<ProgramPadPressEvent> getMostRecentProgramPadPress(
            ProgramPadIndex,
            const std::initializer_list<PerformanceEventSource> &sourcesToExclude) const;

        int getTotalNoteOnCount() const;

        Program getProg(ProgramIndex) const;
        Program getDrumProgram(DrumBusIndex) const;
        Drum getDrum(DrumBusIndex) const;

        std::optional<NoteOnEvent>
        findNoteOnEvent(PerformanceEventSource, NoteNumber,
                        MidiChannel midiInputChannel);

        std::optional<PhysicalPadPressEvent>
            findPhysicalPadPress(PhysicalPadIndex);

    private:
        const PerformanceState *const state;
    };
} // namespace mpc::performance
