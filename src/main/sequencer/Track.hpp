#pragma once

#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/NoteEvent.hpp"
#include "Observer.hpp"

#include <concurrentqueue.h>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{

    class FrameSeq;

    class Track : public Observable
    {

    private:
        mpc::Mpc &mpc;
        static const int MAX_TICK{2147483647};

        std::vector<std::shared_ptr<Event>> events;

        moodycamel::ConcurrentQueue<std::shared_ptr<NoteOnEvent>>
            queuedNoteOnEvents =
                moodycamel::ConcurrentQueue<std::shared_ptr<NoteOnEvent>>(20);
        moodycamel::ConcurrentQueue<std::shared_ptr<NoteOffEvent>>
            queuedNoteOffEvents =
                moodycamel::ConcurrentQueue<std::shared_ptr<NoteOffEvent>>(20);
        std::vector<std::shared_ptr<NoteOnEvent>> bulkNoteOns =
            std::vector<std::shared_ptr<NoteOnEvent>>(20);
        std::vector<std::shared_ptr<NoteOffEvent>> bulkNoteOffs =
            std::vector<std::shared_ptr<NoteOffEvent>>(20);

        Sequence *parent{nullptr};

        int busNumber = 0;
        std::string name;
        bool on{false};
        int velocityRatio = 0;
        int programChange = 0;
        int device = 0;
        int trackIndex = 0;
        bool used{false};
        int eventIndex = 0;

    public:
        void move(int tick, int oldTick);
        void setTrackIndex(int i);
        int getIndex();
        void flushNoteCache();
        void setUsed(bool b);
        void setOn(bool b);

    private:
        std::shared_ptr<NoteOnEvent> getNoteEvent(int tick, int note);
        void processRealtimeQueuedEvents();
        int getCorrectedTickPos();

    public:
        bool insertEventWhileRetainingSort(
            const std::shared_ptr<Event> &event,
            bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);
        std::shared_ptr<NoteOnEvent> recordNoteEventSynced(int tick, int note,
                                                           int velocity);
        bool finalizeNoteEventSynced(const std::shared_ptr<NoteOnEvent> &event,
                                     int duration);
        std::shared_ptr<NoteOnEvent>
        recordNoteEventASync(unsigned char note, unsigned char velocity);
        void finalizeNoteEventASync(const std::shared_ptr<NoteOnEvent> &event);
        void
        addEvent(int tick, const std::shared_ptr<Event> &event,
                 bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);
        void cloneEventIntoTrack(std::shared_ptr<Event> &src, int tick,
                                 bool allowMultipleNotesOnSameTick = false);
        void removeEvent(int i);
        void removeEvent(const std::shared_ptr<Event> &event);
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
        std::vector<std::shared_ptr<Event>> &getEvents();

        int getNextTick();
        void playNext();
        bool isOn();
        bool isUsed();

    public:
        std::vector<std::shared_ptr<Event>> getEventRange(int startTick,
                                                          int endTick);

        // Do not call from audio thread
        void correctTimeRange(int startPos, int endPos, int stepLength,
                              int swingPercentage, int lowestNote,
                              int highestNote);

    public:
        // Do not call from audio thread
        void removeDoubles();

    private:
        void updateEventTick(std::shared_ptr<Event> &e, int newTick);

    public:
        std::vector<std::shared_ptr<NoteOnEvent>> getNoteEvents();
        void timingCorrect(int fromBar, int toBar,
                           const std::shared_ptr<NoteOnEvent> &noteEvent,
                           int stepLength, int swingPercentage);
        int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength,
                              int swingPercentage);

        // void swing(std::vector<std::shared_ptr<Event>>& eventsToSwing, int
        // noteValue, int percentage, std::vector<int>& noteRange);

        void shiftTiming(std::shared_ptr<Event> eventsToShift, bool later,
                         int amount, int lastTick);

        std::string getActualName();

        Track(mpc::Mpc &mpc, Sequence *parent, int i);

    private:
        friend class Sequence;
        friend class FrameSeq;
    };
} // namespace mpc::sequencer
