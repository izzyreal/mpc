#pragma once

#include "performance/PerformanceEventSource.hpp"

#include "IntTypes.hpp"
#include "lcdgui/ScreenId.hpp"
#include "sequencer/BusType.hpp"

#include <vector>
#include <cstdint>

namespace mpc::sequencer
{
    struct EventData;
}
namespace mpc::performance
{
    struct PhysicalPadPressEvent
    {
        PhysicalPadIndex padIndex;
        PerformanceEventSource source;
        lcdgui::ScreenId screenId;
        TrackIndex trackIndex;
        sequencer::BusType busType;
        Velocity velocity;
        ProgramIndex programIndex;
        DrumNoteNumber drumNoteNumber;
        Pressure pressure;
    };

    struct PhysicalPadAftertouchEvent
    {
        PhysicalPadIndex padIndex;
        Pressure pressure;
        PerformanceEventSource source;
    };

    struct PhysicalPadReleaseEvent
    {
        PhysicalPadIndex padIndex;
    };

    struct ProgramPadPressEvent
    {
        ProgramPadIndex padIndex;
        PerformanceEventSource source;
        MidiChannel midiInputChannel;
        lcdgui::ScreenId screenId;
        TrackIndex trackIndex;
        sequencer::BusType busType;
        ProgramIndex programIndex;
        Velocity velocity;
        Pressure pressure;
        TimeInMilliseconds pressTimeMs;
        PhysicalPadIndex physicalPadIndex;
        bool quantizedLockActivated = false;
    };

    struct ProgramPadAftertouchEvent
    {
        ProgramPadIndex padIndex;
        ProgramIndex programIndex;
        Pressure pressure;
        PerformanceEventSource source;
    };

    struct ProgramPadReleaseEvent
    {
        ProgramPadIndex padIndex;
        ProgramIndex programIndex;
        PerformanceEventSource source;
    };

    struct NoteOnEvent
    {
        NoteNumber noteNumber;
        PerformanceEventSource source;
        MidiChannel midiInputChannel;
        lcdgui::ScreenId screenId;
        TrackIndex trackIndex;
        sequencer::BusType busType;
        Velocity velocity;
        ProgramIndex programIndex;
        Pressure pressure;
    };

    struct NoteAftertouchEvent
    {
        NoteNumber noteNumber;
        Pressure pressure;
        MidiChannel midiInputChannel;
        PerformanceEventSource source;
    };

    struct NoteOffEvent
    {
        NoteNumber noteNumber;
        MidiChannel midiInputChannel;
        PerformanceEventSource source;
    };

    using PhysicalPadPressEvents = std::vector<PhysicalPadPressEvent>;
    using ProgramPadPressEvents = std::vector<ProgramPadPressEvent>;
    using NoteOnEvents = std::vector<NoteOnEvent>;
} // namespace mpc::performance
