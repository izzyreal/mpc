#pragma once
#include "IntTypes.hpp"
#include "sequencer/Event.hpp"

#include <memory>
#include <cassert>

namespace mpc::sequencer
{
    class NoteOnEvent final : public Event
    {
        bool beingRecorded = false;
        NoteEventId id;

        // Used when recording in the step editor or in the MAIN screen when the
        // sequencer is not running, and we need to derive duration based on how
        // long the note was pressed, using the metronome-only sequencer (which
        // is also running when Count is OFF).
        int metronomeOnlyTickPosition = 0;

    public:
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot,
        const std::function<void(TrackEventMessage &&)> &dispatch,
                             NoteNumber, Velocity vel = MaxVelocity,
                             NoteEventId = NoNoteEventId);
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch);
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot,
        const std::function<void(TrackEventMessage &&)> &dispatch,
                             DrumNoteNumber);

        void setBeingRecorded(bool);
        bool isBeingRecorded() const;

        void setMetronomeOnlyTickPosition(int pos);

        int getMetronomeOnlyTickPosition() const;

        void setNote(NoteNumber) const;
        NoteNumber getNote() const;
        void setDuration(Duration duration) const;
        Duration getDuration() const;
        void resetDuration() const;
        NoteVariationType getVariationType() const;
        void setVariationType(NoteVariationType) const;
        void setVariationValue(int i) const;
        int getVariationValue() const;
        void setVelocity(Velocity) const;
        Velocity getVelocity() const;

        std::string getTypeName() const override
        {
            return "note-on";
        }

        uint32_t getId() const;
    };
} // namespace mpc::sequencer
