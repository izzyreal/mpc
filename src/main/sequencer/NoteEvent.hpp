#pragma once
#include <sequencer/Event.hpp>
#include <engine/midi/ShortMessage.hpp>
#include <optional>
#include "assert.h"

namespace mpc::sequencer
{
    class NoteOffEvent : public Event
    {
        friend class NoteOnEvent;

    private:
        int number = 60;
        NoteOffEvent(){};
        void setNote(int i);
    public:
        int getNote();
        bool isDrumNote();

        std::string getTypeName() override { return "note-off"; }
        std::shared_ptr<mpc::engine::midi::ShortMessage> createShortMessage(int channel, int transpose = 0);
    };


    class NoteOnEvent : public Event
    {
    public:
        enum VARIATION_TYPE {TUNE_0 = 0, DECAY_1 = 1, ATTACK_2 = 2, FILTER_3 = 3};
        typedef std::optional<int> Duration;
    private:
        int number = 60;
        Duration duration;
        VARIATION_TYPE variationType = VARIATION_TYPE::TUNE_0;
        int variationValue = 64;
        int velocity;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        std::shared_ptr<NoteOffEvent> getNoteOff();
        void setTrack(int track) override;

        void setNote(int i);
        int getNote();
        void setDuration(Duration duration);
        Duration getDuration();
        void resetDuration();
        VARIATION_TYPE getVariationType();
        void incrementVariationType(int amount);
        void setVariationType(VARIATION_TYPE type);
        void setVariationValue(int i);
        int getVariationValue();
        void setVelocity(int i);
        int getVelocity();
        bool isDrumNote();
        bool isFinalized();

        NoteOnEvent(int i = 60, int vel = 127);
        NoteOnEvent(mpc::engine::midi::ShortMessage* msg);
        NoteOnEvent(const NoteOnEvent&);

        std::string getTypeName() override { return "note-on"; }
        std::shared_ptr<mpc::engine::midi::ShortMessage> createShortMessage(int channel, int transpose = 0);
        void CopyValuesTo(std::weak_ptr<Event> dest) override;


    };
    static bool isDrumNote(int number) { return number >= 35 && number <= 98; }

    class NoteOnEventPlayOnly : public NoteOnEvent
    {
    public:
        NoteOnEventPlayOnly(int i = 60, int vel = 127) : NoteOnEvent(i,vel) { setTick(-1); };
        NoteOnEventPlayOnly(mpc::engine::midi::ShortMessage* msg) : NoteOnEvent(msg) { setTick(-1); }
        NoteOnEventPlayOnly(const NoteOnEventPlayOnly& event) : NoteOnEvent(event) { setTick(-1); };
    };
}
inline int operator+(const mpc::sequencer::NoteOnEvent::Duration& duration, const int& i)
{
    assert(duration.has_value());
    return *duration + i;
}
inline int operator+(const int& i, const mpc::sequencer::NoteOnEvent::Duration& duration)
{
    assert(duration.has_value());
    return *duration + i;
}
inline int operator-(const mpc::sequencer::NoteOnEvent::Duration& duration, const int& i)
{
    assert(duration.has_value());
    return *duration - i;
}
inline int operator-(const int& i, const mpc::sequencer::NoteOnEvent::Duration& duration)
{
    assert(duration.has_value());
    return *duration - i;
}
inline int operator*(const mpc::sequencer::NoteOnEvent::Duration& duration, const int& i)
{
    assert(duration.has_value());
    return *duration * i;
}
inline int operator*(const int& i, const mpc::sequencer::NoteOnEvent::Duration& duration)
{
    assert(duration.has_value());
    return *duration * i;
}
inline int operator/(const mpc::sequencer::NoteOnEvent::Duration& duration, const int& i)
{
    assert(duration.has_value());
    return *duration / i;
}
inline int operator/(const int& i, const mpc::sequencer::NoteOnEvent::Duration& duration)
{
    assert(duration.has_value());
    return *duration / i;
}
