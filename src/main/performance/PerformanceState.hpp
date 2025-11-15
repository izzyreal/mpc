#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

namespace mpc::performance
{
    struct PerformanceState
    {
        std::array<Drum, 4> drums;

        PhysicalPadPressEvents physicalPadEvents;
        ProgramPadPressEvents programPadEvents;
        NoteOnEvents noteEvents;

        PerformanceState() = default;

        PerformanceState(const PerformanceState &other) noexcept
            : physicalPadEvents(other.physicalPadEvents),
              programPadEvents(other.programPadEvents),
              noteEvents(other.noteEvents),
              drums(other.drums)
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
