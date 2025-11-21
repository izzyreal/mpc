#pragma once

#include "IntTypes.hpp"
#include "sequencer/EventType.hpp"

namespace mpc::sequencer
{
    struct EventState
    {
        EventType type;
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;

        EventIndex eventIndex;
        EventIndex previousEventIndex;
        EventIndex nextEventIndex;

        Tick tick;

        NoteNumber noteNumber;
        NoteEventId noteEventId;
        bool beingRecorded;
        int wasMoved;
        bool dontDelete;
        int64_t metronomeOnlyTickPosition;

        Duration duration;
        Velocity velocity;
        NoteVariationType noteVariationType;
        NoteVariationValue noteVariationValue;

        int8_t amount;

        int8_t controllerNumber;
        int8_t controllerValue;

        ProgramIndex programChangeProgramIndex;

        int8_t mixerParameter;
        int8_t mixerPad;
        int8_t mixerValue;

        EventState()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            type = EventType::Unknown;
            sequenceIndex = NoSequenceIndex;
            trackIndex = NoTrackIndex;

            eventIndex = NoEventIndex;
            previousEventIndex = NoEventIndex;
            nextEventIndex = NoEventIndex;

            tick = NoTick;

            noteNumber = NoNoteNumber;
            noteEventId = NoNoteEventId;
            beingRecorded = false;

            duration = NoDuration;
            velocity = NoVelocityOrPressure;
            noteVariationType = NoteVariationTypeTune;
            noteVariationValue = DefaultNoteVariationValue;
        }
    };
}
