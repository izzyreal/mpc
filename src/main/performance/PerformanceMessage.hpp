#pragma once

#include "performance/EventTypes.hpp"
#include "performance/Drum.hpp"

#include <functional>
#include <variant>

namespace mpc::performance
{
    struct SetDrumProgram
    {
        DrumBusIndex drumBusIndex;
        performance::Program performanceProgram;
    };

    using PerformanceMessagePayload =
        std::variant<std::monostate, PhysicalPadPressEvent,
                     PhysicalPadAftertouchEvent, PhysicalPadReleaseEvent,
                     ProgramPadPressEvent, ProgramPadAftertouchEvent,
                     ProgramPadReleaseEvent, NoteOnEvent, NoteAftertouchEvent,
                     NoteOffEvent, SetDrumProgram>;

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
