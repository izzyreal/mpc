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

        PhysicalPadPressEvent physicalPadPress;
        PhysicalPadAftertouchEvent physicalPadAftertouch;
        PhysicalPadReleaseEvent physicalPadRelease;

        ProgramPadPressEvent programPadPress;
        ProgramPadAftertouchEvent programPadAftertouch;
        ProgramPadReleaseEvent programPadRelease;

        NoteOnEvent noteOnEvent;
        NoteAftertouchEvent noteAftertouchEvent;
        NoteOffEvent noteOffEvent;

        Source source;
        std::function<void(void *)> action = [](void *) {};

        EventMessage() = default;
        EventMessage(const EventMessage &) = delete;
        EventMessage &operator=(const EventMessage &) = delete;
        EventMessage(EventMessage &&) noexcept = default;
        EventMessage &operator=(EventMessage &&) noexcept = default;
    };
} // namespace mpc::eventregistry
