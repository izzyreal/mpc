#pragma once
#include <sequencer/Event.hpp>

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

        std::string getTypeName() override { return "note-off"; }
    };


    class NoteOnEvent : public Event
    {
    public:
        enum VARIATION_TYPE { TUNE = 0, DECAY = 1, ATTACK = 2, FILTER = 3};
        const int DURATION_UNKNOWN = -1;
    private:
        int number = 60;
        int duration = DURATION_UNKNOWN;
        VARIATION_TYPE variationType = VARIATION_TYPE::TUNE;
        int variationValue = 64;
        int velocity = 127;

    protected:
        std::shared_ptr<NoteOffEvent> noteOff;

    public:
        std::shared_ptr<NoteOffEvent> getNoteOff();

        void setNote(int i);
        int getNote();
        void setDuration(int i);
        int getDuration();
        VARIATION_TYPE getVariationType();
        void setVariationType(VARIATION_TYPE type);
        void setVariationValue(int i);
        int getVariationValue();
        void setVelocity(int i);
        int getVelocity();

        NoteOnEvent(int i = 60);

        std::string getTypeName() override { return "note-on"; }

    };


    class NoteEvent
        : public Event
    {

    private:
        int number = 60;
        int duration = -1;
        int variationTypeNumber = 0;
        int variationValue = 64;
        int velocity = 0;

    protected:
        std::shared_ptr<NoteEvent> noteOff;

    public:
        std::shared_ptr<NoteEvent> getNoteOff();

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

        void CopyValuesTo(std::weak_ptr<Event> dest) override;

        NoteEvent();
        NoteEvent(int i);

        NoteEvent(bool noteOffTrue, int /* noteOnTick */); // ctor used for noteOffs
        std::string getTypeName() override { return "note"; }

    };
}
