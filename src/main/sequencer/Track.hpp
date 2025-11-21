#pragma once

#include "sequencer/BusType.hpp"
#include "IntTypes.hpp"

#include "performance/Sequence.hpp"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <cstdint>

namespace mpc::performance
{
    class PerformanceManager;
}
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
}

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::sequencer
{
    class TrackEventStateManager;
    class Sequence;
    class Event;
    class NoteOnEvent;
    class NoteOffEvent;
    class Bus;

    class Track
    {
    public:
        Track(
            const std::shared_ptr<performance::PerformanceManager> &,
            int trackIndex, Sequence *parent,
            const std::function<std::string(int)> &getDefaultTrackName,
            const std::function<int64_t()> &getTickPosition,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &getScreens,
            const std::function<bool()> &isRecordingModeMulti,
            const std::function<std::shared_ptr<Sequence>()> &getActiveSequence,
            const std::function<int()> &getAutoPunchMode,
            const std::function<std::shared_ptr<Bus>(BusType)> &getSequencerBus,
            const std::function<bool()> &isEraseButtonPressed,
            const std::function<bool(ProgramPadIndex, ProgramIndex)>
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

        // Allocates! Don't invoke on audio thread
        std::vector<std::shared_ptr<NoteOnEvent>> getNoteEvents() const;

        void timingCorrect(int fromBar, int toBar,
                           const std::shared_ptr<NoteOnEvent> &noteEvent,
                           int stepLength, int swingPercentage);

        int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength,
                              int swingPercentage) const;

        void shiftTiming(const std::shared_ptr<Event> &, bool later, int amount,
                         int lastTick);

        std::string getActualName();

        void syncEventIndex(int tick, int oldTick);
        void setTrackIndex(TrackIndex i);
        TrackIndex getIndex() const;
        void flushNoteCache() const;
        void setUsed(bool b);
        void setOn(bool b);

        void insertEventWhileRetainingSort(
            const performance::Event &event,
            bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);

        mpc::performance::Event
        recordNoteEventNonLive(int tick, NoteNumber, Velocity, int64_t metronomeOnlyTick);

        performance::Event recordNoteEventLive(NoteNumber, Velocity);

        // Only to be used for note events that are being recorded while the
        // sequencer is running, i.e. due to live MIDI, mouse, keyboard or
        // other input.
        // For non-live note event recording, i.e. in the step editor and in the
        // MAIN screen when the sequencer is not running, use
        // NoteOnEvent::finalizeNonLive.
        void
        finalizeNoteEventLive(const performance::Event &) const;

        void
        finalizeNoteEventNonLive(const performance::Event &) const;

        void
        addEvent(const performance::Event &,
                 bool allowMultipleNoteEventsWithSameNoteOnSameTick = false);

        void cloneEventIntoTrack(const std::shared_ptr<Event> &, int tick,
                                 bool allowMultipleNotesOnSameTick = false);

        void cloneEventIntoTrack(const performance::Event &,
                                 bool allowMultipleNotesOnSameTick = false);

        void removeEvent(int i);
        void removeEvent(const std::shared_ptr<Event> &event);
        void removeEvents();
        void setVelocityRatio(int i);
        int getVelocityRatio() const;
        void setProgramChange(int i);
        int getProgramChange() const;
        void setBusType(BusType);
        BusType getBusType() const;
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

        performance::Event findRecordingNoteOnEventById(NoteEventId);

        performance::Event
            findRecordingNoteOnEventByNoteNumber(NoteNumber);

    private:
        std::shared_ptr<TrackEventStateManager> eventStateManager;
        BusType busType = BusType::DRUM1;
        std::string name;
        bool on{false};
        int velocityRatio = 0;
        int programChange = 0;
        int device = 0;
        TrackIndex trackIndex{0};
        bool used{false};
        int eventIndex = 0;

        NoteEventId nextNoteEventId{MinNoteEventId};

        std::vector<std::shared_ptr<Event>> events;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            performance::Event,
            moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOnEvents;
        std::shared_ptr<moodycamel::ConcurrentQueue<
            performance::Event,
            moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOffEvents;

        Sequence *parent{nullptr};
        std::shared_ptr<performance::PerformanceManager> performanceManager;
        std::function<std::string(int)> getDefaultTrackName;
        std::function<int64_t()> getTickPosition;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<bool()> isRecordingModeMulti;
        std::function<std::shared_ptr<Sequence>()> getActiveSequence;
        std::function<int()> getAutoPunchMode;
        std::function<std::shared_ptr<Bus>(BusType)> getSequencerBus;
        std::function<bool()> isEraseButtonPressed;
        std::function<bool(ProgramPadIndex, ProgramIndex)> isProgramPadPressed;
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

        std::vector<performance::Event> bulkNoteOns;
        std::vector<performance::Event> bulkNoteOffs;

        void updateEventTick(const std::shared_ptr<Event> &e, int newTick);
        std::shared_ptr<NoteOnEvent> getNoteEvent(int tick, NoteNumber) const;

        void processRealtimeQueuedEvents();
        int getCorrectedTickPos() const;
    };
} // namespace mpc::sequencer
