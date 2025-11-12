#pragma once

#include "eventregistry/EventTypes.hpp"

namespace mpc::eventregistry
{
    struct State
    {
        PhysicalPadPressEvents physicalPadEvents;
        ProgramPadPressEvents programPadEvents;
        NoteOnEvents noteEvents;

        State() = default;

        State(const State &other) noexcept
            : physicalPadEvents(other.physicalPadEvents),
              programPadEvents(other.programPadEvents),
              noteEvents(other.noteEvents)
        {
        }

        State &operator=(const State &other) noexcept
        {
            physicalPadEvents = other.physicalPadEvents;
            programPadEvents = other.programPadEvents;
            noteEvents = other.noteEvents;
            return *this;
        }
    };
} // namespace mpc::eventregistry
