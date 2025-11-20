#pragma once
#include "IntTypes.hpp"
#include "sequencer/Event.hpp"

#include <optional>
#include <memory>
#include <cassert>

namespace mpc::sequencer
{
    bool isDrumNote(NoteNumber);

    class NoteOffEvent final : public Event
    {
        NoteNumber number{60};
    public:
        explicit NoteOffEvent(const std::function<performance::Event()> &getSnapshot, const NoteNumber numberToUse)
            : Event(getSnapshot)
        {
            number = numberToUse;
        }

        explicit NoteOffEvent(const std::function<performance::Event()> &getSnapshot) : Event(getSnapshot) {}
        void setNote(NoteNumber);

        NoteNumber getNote() const;

        std::string getTypeName() const override
        {
            return "note-off";
        }
    };

    class NoteOnEvent : public Event
    {
    public:
        enum VARIATION_TYPE
        {
            TUNE_0 = 0,
            DECAY_1 = 1,
            ATTACK_2 = 2,
            FILTER_3 = 3
        };
        typedef std::optional<int> Duration;

    private:
        bool beingRecorded = false;
        NoteEventId id;
        NoteNumber noteNumber{60};
        Duration duration;
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
        explicit NoteOnEvent(const std::function<performance::Event()> &getSnapshot, NoteNumber, Velocity vel = MaxVelocity,
                             NoteEventId = NoNoteEventId);
        explicit NoteOnEvent(const std::function<performance::Event()> &getSnapshot);
        explicit NoteOnEvent(const std::function<performance::Event()> &getSnapshot, DrumNoteNumber);
        NoteOnEvent(const NoteOnEvent &);
        std::shared_ptr<NoteOffEvent> getNoteOff() const;
        void setTrack(TrackIndex trackIndexToUse) override;

        void setBeingRecorded(bool);
        bool isBeingRecorded() const;

        // Only to be used to finalize note events that are being recorded
        // in the context of a non-running sequencer, i.e. non-live. This is
        // true for recording note events in the step editor, as well is in
        // the MAIN screen when the sequencer is not running.
        // For live note event finalized, use Track::finalizeNoteEventLive.
        bool finalizeNonLive(int newDuration);

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
        bool isPlayOnly();

        std::string getTypeName() const override
        {
            return "note-on";
        }

        uint32_t getId() const;
    };

    class NoteOnEventPlayOnly final : public NoteOnEvent
    {
    public:
        explicit NoteOnEventPlayOnly(const std::function<performance::Event()> &getSnapshot, const NoteNumber i,
                                     const Velocity vel = MaxVelocity)
            : NoteOnEvent(getSnapshot, i, vel)
        {
            setTick(-1);
        }

        NoteOnEventPlayOnly(const NoteOnEventPlayOnly &event)
            : NoteOnEvent(event)
        {
            setTick(-1);
            NoteOnEvent::setTrack(event.track);
        }

        explicit NoteOnEventPlayOnly(const NoteOnEvent &event)
            : NoteOnEvent(event)
        {
            setTick(-1);
            NoteOnEvent::setTrack(event.getTrack());
        }
    };
} // namespace mpc::sequencer

inline int operator+(const mpc::sequencer::NoteOnEvent::Duration &duration,
                     const int &i)
{
    assert(duration.has_value());
    return *duration + i;
}

inline int operator+(const int &i,
                     const mpc::sequencer::NoteOnEvent::Duration &duration)
{
    assert(duration.has_value());
    return *duration + i;
}

inline int operator-(const mpc::sequencer::NoteOnEvent::Duration &duration,
                     const int &i)
{
    assert(duration.has_value());
    return *duration - i;
}

inline int operator*(const mpc::sequencer::NoteOnEvent::Duration &duration,
                     const int &i)
{
    assert(duration.has_value());
    return *duration * i;
}
