#pragma once

#include "eventregistry/Source.hpp"

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
        Source source;
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
        Source source;
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
        Source source;
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
    using PhysicalPadAftertouchEvents = std::vector<PhysicalPadAftertouchEvent>;
    using PhysicalPadReleaseEvents = std::vector<PhysicalPadReleaseEvent>;
    using ProgramPadPressEvents = std::vector<ProgramPadPressEvent>;
    using ProgramPadAftertouchEvents = std::vector<ProgramPadAftertouchEvent>;
    using ProgramPadReleaseEvents = std::vector<ProgramPadReleaseEvent>;
    using NoteOnEvents = std::vector<NoteOnEvent>;
    using NoteAftertouchEvents = std::vector<NoteAftertouchEvent>;
    using NoteOffEvents = std::vector<NoteOffEvent>;
} // namespace mpc::eventregistry
