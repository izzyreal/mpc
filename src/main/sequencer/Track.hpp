#pragma once

#include <Mpc.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>
#include <observer/Observable.hpp>

#include "thirdp/concurrentqueue.h"
#include <memory>
#include <unordered_map>

namespace mpc { class Mpc; }

namespace mpc::sequencer {

    class FrameSeq;

class Track
: public moduru::observer::Observable
{
    
private:
    mpc::Mpc& mpc;
    static const int MAX_TICK{ 2147483647 };
    
    std::vector<std::shared_ptr<Event>> events;

    moodycamel::ConcurrentQueue<std::shared_ptr<NoteOnEvent>> queuedNoteOnEvents;
    moodycamel::ConcurrentQueue<std::shared_ptr<NoteOffEvent>> queuedNoteOffEvents;
    std::vector<std::shared_ptr<NoteOnEvent>> bulkNoteOns = std::vector<std::shared_ptr<NoteOnEvent>>(20);
    std::vector<std::shared_ptr<NoteOffEvent>> bulkNoteOffs = std::vector<std::shared_ptr<NoteOffEvent>>(20);

    mpc::sequencer::Sequence* parent{ nullptr };
    
    std::shared_ptr<Sequencer> sequencer;
    int busNumber = 0;
    std::string name;
    bool on{ false };
    int velocityRatio = 0;
    int programChange = 0;
    int device = 0;
    int trackIndex = 0;
    bool used{ false };
    int eventIndex = 0;
    std::unordered_map<int, std::shared_ptr<mpc::sequencer::NoteOnEvent>> recordNoteStore = {};
    bool multi{ false };
    bool _delete{ false };

public:
    void move(int tick, int oldTick);
    void setTrackIndex(int i);
    int getIndex();
    void flushNoteCache();
    void setUsed(bool b);
    void setOn(bool b);

    // The below 2 methods are threadsafe, intended for the UI
    // thread (keyboard, mouse) to record notes.
    // MIDI will also go through this, in case that runs on a
    // different thread.
    // As a bare minimum the note must be passed in, in order to
    // match against counterpart (off with on and vice versa).
    std::shared_ptr<mpc::sequencer::NoteOnEvent> recordNoteOnNow(unsigned char note);
    void recordNoteOffNow(unsigned char note);

private:
    std::shared_ptr<NoteOnEvent> getNoteEvent(int tick, int note);
    void processRealtimeQueuedEvents();
    int getCorrectedTickPos();

    bool storeRecordNoteEvent(int note, std::shared_ptr<mpc::sequencer::NoteOnEvent> event);
    std::shared_ptr<mpc::sequencer::NoteOnEvent> retrieveRecordNoteEvent(int note);

public:
    bool insertEventWhileRetainingSort(const std::shared_ptr<Event>& event, bool allowMultipleNotesOnSameTick = false);
    std::shared_ptr<NoteOnEvent> addNoteEvent(int tick, int note);
    std::shared_ptr<Event> addEvent(int tick, const std::string& type, bool allowMultipleNotesOnSameTick = false);
    void cloneEventIntoTrack(std::shared_ptr<Event>& src, int tick, bool allowMultipleNotesOnSameTick = false);
    bool finalizeNoteOnEvent(int note, int duration);
    void removeEvent(int i);
    void removeEvent(const std::shared_ptr<Event>& event);
    void removeEvents();
    void setVelocityRatio(int i);
    int getVelocityRatio();
    void setProgramChange(int i);
    int getProgramChange();
    void setBusNumber(int i);
    int getBus();
    void setDeviceIndex(int i);
    int getDeviceIndex();
    std::shared_ptr<Event> getEvent(int i);
    void setName(std::string s);
    std::string getName();
    std::vector<std::shared_ptr<Event>>& getEvents();
    
    int getNextTick();
    void playNext();
    bool isOn();
    bool isUsed();

public:
    std::vector<std::shared_ptr<Event>> getEventRange(int startTick, int endTick);

    // Do not call from audio thread
    void correctTimeRange(int startPos, int endPos, int stepLength, int swingPercentage, int lowestNote, int highestNote);
    
public:
    // Do not call from audio thread
    void removeDoubles();

private:
    std::vector<std::shared_ptr<NoteOnEvent>> getNoteEventsAtTick(int tick);
    void updateEventTick(std::shared_ptr<Event>& e, int newTick);

public:
    std::vector<std::shared_ptr<NoteOnEvent>> getNoteEvents();
    void timingCorrect(int fromBar, int toBar, const std::shared_ptr<NoteOnEvent>& noteEvent, int stepLength, int swingPercentage);
    int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength, int swingPercentage);

    void swing(std::vector<std::shared_ptr<Event>>& eventsToSwing, int noteValue, int percentage, std::vector<int>& noteRange);

    void shiftTiming(std::shared_ptr<Event> eventsToShift, bool later, int amount, int lastTick);

    std::string getActualName();
    
    Track(mpc::Mpc& mpc, mpc::sequencer::Sequence* parent, int i);
    
private:
    friend class Sequence;
    friend class mpc::sequencer::FrameSeq;
    
};
}
