#pragma once

#include "eventregistry/EventTypes.hpp"

namespace mpc::eventregistry
{
    struct PerformanceState
    {
        PhysicalPadPressEvents physicalPadEvents;
        ProgramPadPressEvents programPadEvents;
        NoteOnEvents noteEvents;

        PerformanceState() = default;

        PerformanceState(const PerformanceState &other) noexcept
            : physicalPadEvents(other.physicalPadEvents),
              programPadEvents(other.programPadEvents),
              noteEvents(other.noteEvents)
        {
        }

        PerformanceState &operator=(const PerformanceState &other) noexcept
        {
            physicalPadEvents = other.physicalPadEvents;
            programPadEvents = other.programPadEvents;
            noteEvents = other.noteEvents;
            return *this;
        }
    };
} // namespace mpc::eventregistry
