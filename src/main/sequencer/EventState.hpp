#pragma once

#include "IntTypes.hpp"
#include "sequencer/EventType.hpp"

namespace mpc::sequencer
{
    struct EventState
    {
        // === BASE SECTION ===
        EventType type;
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

        // === END OF RECORD SECTION ===

        // === NOTE AND POLY PRESSURE SECTION ===
        NoteNumber noteNumber;

        // NOTE ON SECTION
        NoteEventId noteEventId;
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

        EventState()
        {
            resetToDefaultValues();
        }

        void resetToDefaultValues()
        {
            type = EventType::Unknown;
            trackIndex = NoTrackIndex;

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
            return type == other.type && trackIndex == other.trackIndex &&
                   tick == other.tick && noteNumber == other.noteNumber &&
                   noteEventId == other.noteEventId &&
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
    };
} // namespace mpc::sequencer
