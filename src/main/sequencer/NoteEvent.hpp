#pragma once
#include <sequencer/Event.hpp>

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
    int noteOnTick = -1;
    
protected:
    std::shared_ptr<NoteEvent> noteOff;
    
public:
    std::weak_ptr<NoteEvent> getNoteOff();
    int getNoteOnTick();
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
    void setTick(int tick) override;
    
    NoteEvent();
    NoteEvent(int i);
    
    NoteEvent(bool noteOffTrue, int noteOnTick); // ctor used for noteOffs
    std::string getTypeName() override { return "note"; }
    
};
}
