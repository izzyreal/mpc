#pragma once

#include <Mpc.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>
#include <observer/Observable.hpp>

#include <memory>

namespace mpc { class Mpc; }

namespace mpc::sequencer {

class Track
: public moduru::observer::Observable
{
    
private:
    mpc::Mpc& mpc;
    static const int MAX_TICK{ 2147483647 };
    
    std::vector<std::shared_ptr<Event>> events;
    
    std::vector<std::shared_ptr<NoteEvent>> queuedNoteOnEvents;
    std::vector<std::shared_ptr<NoteEvent>> noteOffs;
    
    mpc::sequencer::Sequence* parent{ nullptr };
    
    std::weak_ptr<Sequencer> sequencer;
    int busNumber = 0;
    std::string name = "";
    bool on{ false };
    int velocityRatio = 0;
    int programChange = 0;
    int device = 0;
    int trackIndex = 0;
    bool used{ false };
    int eventIndex = 0;
    std::weak_ptr<NoteEvent> lastAdded;
    bool multi{ false };
    bool _delete{ false };
    std::vector<std::weak_ptr<Event>> tempEvents;
    
protected:
    static bool tickCmp(std::weak_ptr<Event> a, std::weak_ptr<Event> b);
    static bool noteCmp(std::weak_ptr<Event> a, std::weak_ptr<Event> b);
    
public:
    void move(int tick, int oldTick);
    void setTrackIndex(int i);
    int getIndex();
    std::weak_ptr<mpc::sequencer::NoteEvent> recordNoteOn();
    void flushNoteCache();
    void recordNoteOff(NoteEvent&);
    void setUsed(bool b);
    void setOn(bool b);
    
private:
    void addEventRealTime(std::shared_ptr<NoteEvent> event);
    std::weak_ptr<NoteEvent> getNoteEvent(int tick, int note);
    
public:
    std::weak_ptr<NoteEvent> addNoteEvent(int tick, int note);
    std::weak_ptr<Event> addEvent(int tick, std::string type);
    std::weak_ptr<Event> cloneEvent(std::weak_ptr<Event> src);
    bool adjustDurLastEvent(int newDur);
    void removeEvent(int i);
    void removeEvent(std::weak_ptr<Event> event);
    void removeEvents();
    void setVelocityRatio(int i);
    int getVelocityRatio();
    void setProgramChange(int i);
    int getProgramChange();
    void setBusNumber(int i);
    int getBus();
    void setDeviceNumber(int i);
    int getDevice();
    std::weak_ptr<Event> getEvent(int i);
    void setName(std::string s);
    std::string getName();
    std::vector<std::weak_ptr<Event>> getEvents();
    
    int getNextTick();
    void playNext();
    bool isOn();
    bool isUsed();
    
public:
    void setEventIndex(int i);
    
public:
    std::vector<std::weak_ptr<Event>> getEventRange(int startTick, int endTick);
    void correctTimeRange(int startPos, int endPos, int stepLength);
    
public:
    void removeDoubles();
    void sortEvents();
    std::vector<std::weak_ptr<NoteEvent>> getNoteEvents();
    
private:
    std::vector<std::weak_ptr<NoteEvent>> getNoteEventsAtTick(int tick);

public:
    void timingCorrect(int fromBar, int toBar, NoteEvent* noteEvent, int stepLength);
    int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength);
    int swingTick(int tick, int noteValue, int percentage);

    void swing(std::vector<std::weak_ptr<Event>> eventsToSwing, int noteValue, int percentage, std::vector<int> noteRange);

    void shiftTiming(std::vector<std::weak_ptr<Event>> eventsToShift, bool later, int amount, int lastTick);
    
public:
    int getEventIndex();
    
public:
    std::string getActualName();
    
public:
    Track(mpc::Mpc& mpc, mpc::sequencer::Sequence* parent, int i);
    
private:
    friend class Sequence;
    
};
}
