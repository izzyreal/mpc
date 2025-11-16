#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <functional>
#include <variant>

namespace mpc::performance
{
    struct UpdateDrumProgram
    {
        DrumBusIndex drumBusIndex;
        ProgramIndex programIndex;
        Program performanceProgram;
    };

    struct UpdateNoteParameters
    {
        ProgramIndex programIndex;
        DrumNoteNumber drumNoteNumber;
        NoteParameters performanceNoteParameters;
    };

    using PerformanceMessagePayload =
        std::variant<std::monostate, PhysicalPadPressEvent,
                     PhysicalPadAftertouchEvent, PhysicalPadReleaseEvent,
                     ProgramPadPressEvent, ProgramPadAftertouchEvent,
                     ProgramPadReleaseEvent, NoteOnEvent, NoteAftertouchEvent,
                     NoteOffEvent, UpdateDrumProgram,
                     UpdateNoteParameters>;

    struct PerformanceMessage
    {
        PerformanceMessagePayload payload;
        PerformanceEventSource source;
        std::function<void(void *)> action = [](void *) {};

        PerformanceMessage() = default;
        PerformanceMessage(const PerformanceMessage &) = delete;
        PerformanceMessage &operator=(const PerformanceMessage &) = delete;
        PerformanceMessage(PerformanceMessage &&) noexcept = default;
        PerformanceMessage &operator=(PerformanceMessage &&) noexcept = default;
    };
} // namespace mpc::performance
