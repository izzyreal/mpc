#pragma once
#include "sequencer/Event.hpp"

#include <optional>
#include <memory>
#include <cassert>

namespace mpc::sequencer
{
    bool isDrumNote(int number);

    class NoteOffEvent final : public Event
    {
        friend class NoteOnEvent;

        int number = 60;
        NoteOffEvent() {}
        void setNote(int i);

    public:
        NoteOffEvent(const int numberToUse)
        {
            number = numberToUse;
        }
        int getNote() const;

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
        int number = 60;
        Duration duration;
        VARIATION_TYPE variationType = VARIATION_TYPE::TUNE_0;
        int variationValue = 64;
        int velocity;

        // Used when recording in the step editor or in the MAIN screen when the
        // sequencer is not running, and we need to derive duration based on how
        // long the note was pressed, using the metronome-only sequencer (which
        // is also running when Count is OFF).
        int metronomeOnlyTickPosition = 0;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        std::shared_ptr<NoteOffEvent> getNoteOff() const;
        void setTrack(int track) override;

        void setMetrononomeOnlyTickPosition(const int pos)
        {
            metronomeOnlyTickPosition = pos;
        }

        int getMetronomeOnlyTickPosition() const
        {
            return metronomeOnlyTickPosition;
        }

        void setNote(int i);
        int getNote() const;
        void setDuration(Duration duration);
        Duration getDuration() const;
        void resetDuration();
        VARIATION_TYPE getVariationType() const;
        void incrementVariationType(int amount);
        void setVariationType(VARIATION_TYPE type);
        void setVariationValue(int i);
        int getVariationValue() const;
        void setVelocity(int i);
        int getVelocity() const;
        bool isFinalized() const;
        bool isPlayOnly();

        NoteOnEvent(int i = 60, int vel = 127);
        NoteOnEvent(const NoteOnEvent &);

        std::string getTypeName() const override
        {
            return "note-on";
        }
    };

    class NoteOnEventPlayOnly : public NoteOnEvent
    {
    public:
        NoteOnEventPlayOnly(const int i = 60, const int vel = 127)
            : NoteOnEvent(i, vel)
        {
            setTick(-1);
        }
        NoteOnEventPlayOnly(const NoteOnEventPlayOnly &event)
            : NoteOnEvent(event)
        {
            setTick(-1);
        }
        NoteOnEventPlayOnly(const NoteOnEvent &event) : NoteOnEvent(event)
        {
            setTick(-1);
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
