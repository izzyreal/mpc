#pragma once

#include "performance/EventTypes.hpp"

#include <functional>

namespace mpc::performance
{
    struct PerformanceMessage
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

        PerformanceEventSource source;
        std::function<void(void *)> action = [](void *) {};

        PerformanceMessage() = default;
        PerformanceMessage(const PerformanceMessage &) = delete;
        PerformanceMessage &operator=(const PerformanceMessage &) = delete;
        PerformanceMessage(PerformanceMessage &&) noexcept = default;
        PerformanceMessage &operator=(PerformanceMessage &&) noexcept = default;
    };
} // namespace mpc::performance
