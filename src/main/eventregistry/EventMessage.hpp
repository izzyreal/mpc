#pragma once

#include "eventregistry/EventTypes.hpp"

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
        PhysicalPadAftertouchEventPtr physicalPadAftertouch;
        PhysicalPadReleaseEventPtr physicalPadRelease;

        ProgramPadPressEventPtr programPadPress;
        ProgramPadAftertouchEventPtr programPadAftertouch;
        ProgramPadReleaseEventPtr programPadRelease;

        NoteOnEventPtr noteOnEvent;
        NoteAftertouchEventPtr noteAftertouchEvent;
        NoteOffEventPtr noteOffEvent;

        Source source;
        std::function<void(void *)> action = [](void *) {};

        EventMessage() = default;
        EventMessage(const EventMessage &) = delete;
        EventMessage &operator=(const EventMessage &) = delete;
        EventMessage(EventMessage &&) noexcept = default;
        EventMessage &operator=(EventMessage &&) noexcept = default;
    };
} // namespace mpc::eventregistry
