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

        PerformanceState()
        {
            drums[0].drumBusIndex = DrumBusIndex(0);
            drums[1].drumBusIndex = DrumBusIndex(1);
            drums[2].drumBusIndex = DrumBusIndex(2);
            drums[3].drumBusIndex = DrumBusIndex(3);
        }

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
