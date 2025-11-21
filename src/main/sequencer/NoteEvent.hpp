#pragma once
#include "IntTypes.hpp"
#include "sequencer/Event.hpp"

#include <memory>
#include <cassert>

namespace mpc::sequencer
{
    bool isDrumNote(NoteNumber);

    class NoteOffEvent final : public Event
    {
    public:
        explicit NoteOffEvent(const std::function<EventState()> &getSnapshotToUse, const NoteNumber numberToUse)
            : Event(getSnapshotToUse)
        {
            setNote(numberToUse);
        }

        explicit NoteOffEvent(const std::function<EventState()> &getSnapshotToUse) : Event(getSnapshotToUse) {}

        void setNote(NoteNumber);

        NoteNumber getNote() const;

        std::string getTypeName() const override
        {
            return "note-off";
        }
    };

    class NoteOnEvent final : public Event
    {
        bool beingRecorded = false;
        NoteEventId id;

        // Used when recording in the step editor or in the MAIN screen when the
        // sequencer is not running, and we need to derive duration based on how
        // long the note was pressed, using the metronome-only sequencer (which
        // is also running when Count is OFF).
        int metronomeOnlyTickPosition = 0;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot, NoteNumber, Velocity vel = MaxVelocity,
                             NoteEventId = NoNoteEventId);
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot);
        explicit NoteOnEvent(const std::function<EventState()> &getSnapshot, DrumNoteNumber);

        std::shared_ptr<NoteOffEvent> getNoteOff() const;
        void setTrack(TrackIndex trackIndexToUse) override;

        void setBeingRecorded(bool);
        bool isBeingRecorded() const;

        void setMetronomeOnlyTickPosition(int pos);

        int getMetronomeOnlyTickPosition() const;

        void setNote(NoteNumber);
        NoteNumber getNote() const;
        void setDuration(Duration duration);
        Duration getDuration() const;
        void resetDuration();
        NoteVariationType getVariationType() const;
        void incrementVariationType(int amount);
        void setVariationType(NoteVariationType);
        void setVariationValue(int i);
        int getVariationValue() const;
        void setVelocity(Velocity);
        Velocity getVelocity() const;
        bool isFinalized() const;

        std::string getTypeName() const override
        {
            return "note-on";
        }

        uint32_t getId() const;
    };
} // namespace mpc::sequencer
