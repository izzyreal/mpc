#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <cstdint>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

namespace mpc::lcdgui
{
    class Screens;
}

namespace mpc::sampler
{
    class Sampler;
    class Program;
} // namespace mpc::sampler

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::sequencer
{
    class Sequence;
    class Event;
    class NoteOnEvent;
    class NoteOffEvent;
    class Bus;

    class Track
    {
    public:
        Track(
            const int trackIndex, Sequence *parent,
            const std::function<std::string(int)> &getDefaultTrackName,
            const std::function<int64_t()> &getTickPosition,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &getScreens,
            const std::function<bool()> &isRecordingModeMulti,
            const std::function<std::shared_ptr<Sequence>()> &getActiveSequence,
            const std::function<int()> &getAutoPunchMode,
            const std::function<std::shared_ptr<Bus>(int)> &getSequencerBus,
            const std::function<bool()> &isEraseButtonPressed,
            const std::function<bool(int programPadIndex,
                                     std::shared_ptr<sampler::Program>)>
                &isProgramPadPressed,
            const std::shared_ptr<sampler::Sampler> &sampler,
            const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
            const std::function<bool()> &isSixteenLevelsEnabled,
            const std::function<int()> &getActiveTrackIndex,
            const std::function<bool()> &isRecording,
            const std::function<bool()> &isOverdubbing,
            const std::function<bool()> &isPunchEnabled,
            const std::function<int64_t()> &getPunchInTime,
            const std::function<int64_t()> &getPunchOutTime,
            const std::function<bool()> &isSoloEnabled);

        std::vector<std::shared_ptr<NoteOnEvent>> getNoteEvents() const;

        void timingCorrect(int fromBar, int toBar,
                           const std::shared_ptr<NoteOnEvent> &noteEvent,
                           int stepLength, int swingPercentage);

        int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength,
                              int swingPercentage) const;

        void shiftTiming(std::shared_ptr<Event> eventsToShift, bool later,
                         int amount, int lastTick);

        std::string getActualName();

        void move(int tick, int oldTick);
        void setTrackIndex(int i);
        int getIndex() const;
        void flushNoteCache() const;
        void setUsed(bool b);
        void setOn(bool b);

        bool insertEventWhileRetainingSort(
            const std::shared_ptr<Event> &event,
            bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);

        std::shared_ptr<NoteOnEvent> recordNoteEventSynced(int tick, int note,
                                                           int velocity);

        bool finalizeNoteEventSynced(const std::shared_ptr<NoteOnEvent> &event,
                                     int duration) const;
        std::shared_ptr<NoteOnEvent>
        recordNoteEventASync(unsigned char note, unsigned char velocity);

        void
        finalizeNoteEventASync(const std::shared_ptr<NoteOnEvent> &event) const;

        void
        addEvent(int tick, const std::shared_ptr<Event> &event,
                 bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);

        void cloneEventIntoTrack(std::shared_ptr<Event> &src, int tick,
                                 bool allowMultipleNotesOnSameTick = false);

        void removeEvent(int i);
        void removeEvent(const std::shared_ptr<Event> &event);
        void removeEvents();
        void setVelocityRatio(int i);
        int getVelocityRatio() const;
        void setProgramChange(int i);
        int getProgramChange() const;
        void setBusNumber(int i);
        int getBus() const;
        void setDeviceIndex(int i);
        int getDeviceIndex() const;
        std::shared_ptr<Event> getEvent(int i);
        void setName(const std::string &s);
        std::string getName();
        std::vector<std::shared_ptr<Event>> &getEvents();

        int getNextTick();
        void playNext();
        bool isOn() const;
        bool isUsed() const;

        std::vector<std::shared_ptr<Event>> getEventRange(int startTick,
                                                          int endTick) const;

        // Do not call from audio thread
        void correctTimeRange(int startPos, int endPos, int stepLength,
                              int swingPercentage, int lowestNote,
                              int highestNote);

        // Do not call from audio thread
        void removeDoubles();

        void purge();

    private:
        static const int MAX_TICK{2147483647};

        int busNumber = 0;
        std::string name;
        bool on{false};
        int velocityRatio = 0;
        int programChange = 0;
        int device = 0;
        int trackIndex = 0;
        bool used{false};
        int eventIndex = 0;

        std::vector<std::shared_ptr<Event>> events;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            std::shared_ptr<NoteOnEvent>,
            moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOnEvents;
        std::shared_ptr<moodycamel::ConcurrentQueue<
            std::shared_ptr<NoteOffEvent>,
            moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOffEvents;

        Sequence *parent{nullptr};
        std::function<std::string(int)> getDefaultTrackName;
        std::function<int64_t()> getTickPosition;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<bool()> isRecordingModeMulti;
        std::function<std::shared_ptr<Sequence>()> getActiveSequence;
        std::function<int()> getAutoPunchMode;
        std::function<std::shared_ptr<Bus>(int)> getSequencerBus;
        std::function<bool()> isEraseButtonPressed;
        std::function<bool(int programPadIndex,
                           std::shared_ptr<sampler::Program>)>
            isProgramPadPressed;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::function<bool()> isSixteenLevelsEnabled;
        std::function<int()> getActiveTrackIndex;
        std::function<bool()> isRecording;
        std::function<bool()> isOverdubbing;
        std::function<bool()> isPunchEnabled;
        std::function<int64_t()> getPunchInTime;
        std::function<int64_t()> getPunchOutTime;
        std::function<bool()> isSoloEnabled;

        std::vector<std::shared_ptr<NoteOnEvent>> bulkNoteOns;
        std::vector<std::shared_ptr<NoteOffEvent>> bulkNoteOffs;

        void updateEventTick(std::shared_ptr<Event> &e, int newTick);
        std::shared_ptr<NoteOnEvent> getNoteEvent(int tick, int note) const;

        void processRealtimeQueuedEvents();
        int getCorrectedTickPos();
    };
} // namespace mpc::sequencer
