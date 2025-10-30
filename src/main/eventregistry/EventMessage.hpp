#pragma once

#include "IntTypes.hpp"

#include "eventregistry/EventTypes.hpp"

#include <optional>
#include <functional>

namespace mpc::eventregistry
{
    struct EventMessage
    {
        enum class Type
        {
            PhysicalPadPress,
            PhysicalPadAftertouch,
            PhysicalPadRelease,
            ProgramPadPress,
            ProgramPadAftertouch,
            ProgramPadRelease,
            NoteOn,
            NoteAftertouch,
            NoteOff,
            Clear
        } type;

        PhysicalPadPressEventPtr physicalPadPress;
        ProgramPadEventPtr programPad;
        NoteEventPtr noteEvent;
        PhysicalPadReleaseEventPtr physicalPadRelease;

        std::optional<Pressure> pressure;
        std::function<void(void *)> action = [](void *) {};

        EventMessage() = default;
        EventMessage(const EventMessage&) = delete;
        EventMessage& operator=(const EventMessage&) = delete;
        EventMessage(EventMessage&&) noexcept = default;
        EventMessage& operator=(EventMessage&&) noexcept = default;
    };
}

