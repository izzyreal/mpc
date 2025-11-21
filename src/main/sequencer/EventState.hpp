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

            tick = NoTick;

            noteNumber = NoNoteNumber;
            noteEventId = NoNoteEventId;
            beingRecorded = false;

            duration = NoDuration;
            velocity = NoVelocityOrPressure;
            noteVariationType = NoteVariationTypeTune;
            noteVariationValue = DefaultNoteVariationValue;
        }

        bool operator==(EventState const &other) const
        {
            return type == other.type && sequenceIndex == other.sequenceIndex &&
                   trackIndex == other.trackIndex &&
                   eventIndex == other.eventIndex && tick == other.tick &&
                   noteNumber == other.noteNumber &&
                   noteEventId == other.noteEventId &&
                   beingRecorded == other.beingRecorded &&
                   wasMoved == other.wasMoved &&
                   dontDelete == other.dontDelete &&
                   metronomeOnlyTickPosition ==
                       other.metronomeOnlyTickPosition &&
                   duration == other.duration && velocity == other.velocity &&
                   noteVariationType == other.noteVariationType &&
                   noteVariationValue == other.noteVariationValue &&
                   amount == other.amount &&
                   controllerNumber == other.controllerNumber &&
                   controllerValue == other.controllerValue &&
                   programChangeProgramIndex ==
                       other.programChangeProgramIndex &&
                   mixerParameter == other.mixerParameter &&
                   mixerPad == other.mixerPad && mixerValue == other.mixerValue;
        }
    };
} // namespace mpc::sequencer
