#pragma once

#include "performance/PerformanceEventSource.hpp"

#include "IntTypes.hpp"
#include "controller/Bank.hpp"
#include "lcdgui/ScreenId.hpp"
#include "sequencer/BusType.hpp"

#include <vector>
#include <cstdint>

namespace mpc::eventregistry
{
    struct PhysicalPadPressEvent
    {
        PhysicalPadIndex padIndex;
        PerformanceEventSource source;
        lcdgui::ScreenId screenId;
        TrackIndex trackIndex;
        sequencer::BusType busType;
        Velocity velocity;
        controller::Bank bank;
        ProgramIndex programIndex;
        NoteNumber noteNumber;
        Pressure pressure;
    };

    struct PhysicalPadAftertouchEvent
    {
        PhysicalPadIndex padIndex;
        Pressure pressure;
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
    };

    struct ProgramPadAftertouchEvent
    {
        ProgramPadIndex padIndex;
        ProgramIndex programIndex;
        Pressure pressure;
    };

    struct ProgramPadReleaseEvent
    {
        ProgramPadIndex padIndex;
        ProgramIndex programIndex;
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
        NoteEventId recordNoteEventId = NoNoteEventId;
    };

    struct NoteAftertouchEvent
    {
        NoteNumber noteNumber;
        Pressure pressure;
        MidiChannel midiInputChannel;
    };

    struct NoteOffEvent
    {
        NoteNumber noteNumber;
        MidiChannel midiInputChannel;
    };

    using PhysicalPadPressEvents = std::vector<PhysicalPadPressEvent>;
    using ProgramPadPressEvents = std::vector<ProgramPadPressEvent>;
    using NoteOnEvents = std::vector<NoteOnEvent>;
} // namespace mpc::eventregistry
