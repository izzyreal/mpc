#pragma once
#include "IntTypes.hpp"
#include "MpcMacros.hpp"
#include "sequencer/Event.hpp"

#include <memory>
#include <cassert>

namespace mpc::sequencer
{
    bool isDrumNote(NoteNumber);

    class NoteOffEvent final : public Event
    {
    public:
        explicit NoteOffEvent(const std::function<sequencer::EventState()> &getSnapshot, const NoteNumber numberToUse)
            : Event(getSnapshot)
        {
            setNote(numberToUse);
        }

        explicit NoteOffEvent(const std::function<sequencer::EventState()> &getSnapshot) : Event(getSnapshot) {}

        void setNote(NoteNumber);

        NoteNumber getNote() const;

        std::string getTypeName() const override
        {
            return "note-off";
        }
    };

    class NoteOnEvent final : public Event
    {
    public:
        enum VARIATION_TYPE
        {
            TUNE_0 = 0,
            DECAY_1 = 1,
            ATTACK_2 = 2,
            FILTER_3 = 3
        };

    private:
        bool beingRecorded = false;
        NoteEventId id;
        VARIATION_TYPE variationType = TUNE_0;
        int variationValue = 64;
        Velocity velocity;

        // Used when recording in the step editor or in the MAIN screen when the
        // sequencer is not running, and we need to derive duration based on how
        // long the note was pressed, using the metronome-only sequencer (which
        // is also running when Count is OFF).
        int metronomeOnlyTickPosition = 0;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        explicit NoteOnEvent(const std::function<sequencer::EventState()> &getSnapshot, NoteNumber, Velocity vel = MaxVelocity,
                             NoteEventId = NoNoteEventId);
        explicit NoteOnEvent(const std::function<sequencer::EventState()> &getSnapshot);
        explicit NoteOnEvent(const std::function<sequencer::EventState()> &getSnapshot, DrumNoteNumber);

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
        VARIATION_TYPE getVariationType() const;
        void incrementVariationType(int amount);
        void setVariationType(VARIATION_TYPE type);
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
