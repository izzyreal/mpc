#pragma once

#include "IntTypes.hpp"
#include "sequencer/EventType.hpp"

namespace mpc::sequencer
{
    struct EventData
    {
        EventData *prev;
        EventData *next;

        // === BASE SECTION ===
        EventType type;
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;
        Tick tick;

        // === RECORD SECTION ===

        // metronomeOnlyTickPosition is used when recording in the step editor
        // or in the MAIN screen when the sequencer is not running, and we need
        // to derive duration based on how long the note was pressed, using the
        // metronome-only sequencer (which is also running when Count is OFF).
        int64_t metronomeOnlyTickPosition;

        // This event is in the process of being recorded. It should not be
        // considered for any operations apart from finalization. Currently, the
        // only event type that can be in this state, is NoteOn.
        bool beingRecorded;

        // Not strictly necessary over here. Should be moved to local state in
        // `Track::processRealtimeQueuedEvents`.
        int wasMoved;

        bool dontDelete;

        // === END OF RECORD SECTION ===

        // === NOTE AND POLY PRESSURE SECTION ===
        NoteNumber noteNumber;

        // NOTE ON SECTION
        Duration duration;
        Velocity velocity;
        NoteVariationType noteVariationType;
        NoteVariationValue noteVariationValue;

        // PITCH BEND, POLY PRESSURE, CHANNEL PRESSURE,
        // AND TEMPO CHANGE AMOUNT
        int16_t amount;

        int8_t controllerNumber;
        int8_t controllerValue;

        ProgramIndex programChangeProgramIndex;

        int8_t mixerParameter;
        int8_t mixerPad;
        int8_t mixerValue;

        EventData()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            prev = nullptr;
            next = nullptr;
            type = EventType::Unknown;
            sequenceIndex = NoSequenceIndex;
            trackIndex = NoTrackIndex;
            tick = NoTick;
            metronomeOnlyTickPosition = 0;
            beingRecorded = false;
            dontDelete = false;
            wasMoved = 0;
            noteNumber = NoNoteNumber;
            duration = NoDuration;
            velocity = NoVelocityOrPressure;
            noteVariationType = NoteVariationTypeTune;
            noteVariationValue = DefaultNoteVariationValue;
            amount = 0;
            controllerNumber = 0;
            controllerValue = 0;
            programChangeProgramIndex = NoProgramIndex;
            mixerParameter = 0;
            mixerPad = 0;
            mixerValue = 0;
        }

        bool operator==(EventData const &other) const
        {
            return type == other.type && trackIndex == other.trackIndex &&
                   sequenceIndex == other.sequenceIndex && tick == other.tick &&
                   noteNumber == other.noteNumber &&
                   beingRecorded == other.beingRecorded &&
                   wasMoved == other.wasMoved &&
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

        bool operator!=(EventData const &other) const
        {
            return !(*this == other);
        }

        void printInfo() const
        {
            printf("== EventState ==\n");
            printf("     type: %s\n", eventTypeToString(type).c_str());
            printf(" sequence: %i\n", sequenceIndex.get());
            printf("    track: %i\n", trackIndex.get());
            printf("     tick: %ld\n", tick);

            if (type == EventType::NoteOn)
            {
                printf("     note: %i\n", noteNumber.get());
                printf(" velocity: %i\n", velocity.get());
                printf(" duration: %i\n", duration.get());
            }
            printf("================\n");
        }

        void copyNoteEventPropertiesFrom(const EventData *const other)
        {
            duration = other->duration;
            velocity = other->velocity;
            noteVariationType = other->noteVariationType;
            noteVariationValue = other->noteVariationValue;
        }
    };
} // namespace mpc::sequencer
