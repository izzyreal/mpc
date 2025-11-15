#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

namespace mpc::performance
{
    struct PerformanceState
    {
        PhysicalPadPressEvents physicalPadEvents;
        ProgramPadPressEvents programPadEvents;
        NoteOnEvents noteEvents;

        std::array<Drum, 4> drums;

        PerformanceState() = default;

        PerformanceState(const PerformanceState &other) noexcept
            : physicalPadEvents(other.physicalPadEvents),
              programPadEvents(other.programPadEvents),
              noteEvents(other.noteEvents), drums(other.drums)
        {
        }

        PerformanceState &operator=(const PerformanceState &other) noexcept
        {
            physicalPadEvents = other.physicalPadEvents;
            programPadEvents = other.programPadEvents;
            noteEvents = other.noteEvents;
            drums = other.drums;
            return *this;
        }
    };
} // namespace mpc::performance
