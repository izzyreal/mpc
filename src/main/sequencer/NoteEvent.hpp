#pragma once
#include <sequencer/Event.hpp>
#include <engine/midi/ShortMessage.hpp>

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
        static const int DURATION_UNKNOWN = -1;
    private:
        int number = 60;
        int duration = DURATION_UNKNOWN;
        VARIATION_TYPE variationType = VARIATION_TYPE::TUNE_0;
        int variationValue = 64;
        int velocity;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        std::shared_ptr<NoteOffEvent> getNoteOff();

        void setNote(int i);
        int getNote();
        void setDuration(int i);
        int getDuration();
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

        std::string getTypeName() override { return "note-on"; }
        std::shared_ptr<mpc::engine::midi::ShortMessage> createShortMessage(int channel, int transpose = 0);

    };
    static bool isDrumNote(int number) { return number >= 35 && number <= 98; }

    class NoteOnEventPlayOnly : public NoteOnEvent
    {
    public:
        NoteOnEventPlayOnly(int i = 60, int vel = 127) : NoteOnEvent(i,vel) { setTick(-1); };
    };


    class OldNoteEvent
        : public Event
    {

    private:
        int number = 60;
        int duration = -1;
        int variationTypeNumber = 0;
        int variationValue = 64;
        int velocity = 0;

    protected:
        std::shared_ptr<OldNoteEvent> noteOff;

    public:
        std::shared_ptr<OldNoteEvent> getNoteOff();

        void setNote(int i);
        int getNote();
        void setDuration(int i);
        int getDuration();
        int getVariationType();
        void setVariationTypeNumber(int i);
        void setVariationValue(int i);
        int getVariationValue();
        void setVelocity(int i);
        void setVelocityZero();
        int getVelocity();
        bool isNoteOn() { return getVelocity() > 0; }
        bool isNoteOff() { return getVelocity() == 0; }
        bool isDrumNote();

        void CopyValuesTo(std::weak_ptr<Event> dest) override;

        OldNoteEvent();
        OldNoteEvent(int i);

        OldNoteEvent(bool noteOffTrue, int /* noteOnTick */); // ctor used for noteOffs
        std::string getTypeName() override { return "note"; }

        std::shared_ptr<mpc::engine::midi::ShortMessage> createShortMessage(int channel, int transpose = 0);
    };
}
