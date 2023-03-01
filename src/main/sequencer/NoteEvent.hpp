#pragma once
#include <sequencer/Event.hpp>
#include <midi/core/ShortMessage.hpp>

namespace mpc::sequencer
{
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
    bool isNoteOn() { return getVelocity() > 0; }
    bool isNoteOff() { return getVelocity() == 0; }
    
    void CopyValuesTo(std::weak_ptr<Event> dest) override;

    NoteEvent();
    NoteEvent(int i);
    
    NoteEvent(bool noteOffTrue, int /* noteOnTick */); // ctor used for noteOffs
    std::string getTypeName() override { return "note"; }
    
    std::shared_ptr<ctoot::midi::core::ShortMessage> createShortMessage(int channel, int transpose = 0);
    
};
}
