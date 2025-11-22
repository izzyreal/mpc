#pragma once

#include "sequencer/BusType.hpp"
#include "IntTypes.hpp"
#include "TrackEventMessage.hpp"

#include "sequencer/EventState.hpp"

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
    class Bus;

    class Track
    {
    public:
        Track(
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
                           const std::pair<EventIndex, EventState> &noteEvent,
                           int stepLength, int swingPercentage) const;

        int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength,
                              int swingPercentage) const;

        void shiftTiming(const std::shared_ptr<Event> &, bool later, int amount,
                         int lastTick) const;

        std::string getActualName();

        void syncEventIndex(int currentTick, int previousTick);
        void setTrackIndex(TrackIndex i);
        TrackIndex getIndex() const;
        void flushNoteCache() const;
        void setUsed(bool b);
        void setOn(bool b);

        void insertEvent(
            const EventState &event,
            bool allowMultipleNoteEventsWithSameNoteOnSameTick = false,
            const std::function<void()> &onComplete = [] {});

        EventState recordNoteEventNonLive(int tick, NoteNumber, Velocity,
                                          int64_t metronomeOnlyTick = 0);

        EventState recordNoteEventLive(NoteNumber, Velocity);

        // Only to be used for note events that are being recorded while the
        // sequencer is running, i.e. due to live MIDI, mouse, keyboard or
        // other input.
        // For non-live note event recording, i.e. in the step editor and in the
        // MAIN screen when the sequencer is not running, use
        // finalizeNoteEventNonLive.
        void finalizeNoteEventLive(const EventState &) const;

        void finalizeNoteEventNonLive(const EventState &, Duration) const;

        void removeEvent(int i) const;
        void removeEvent(const std::shared_ptr<Event> &event) const;
        void removeEvents();
        void setVelocityRatio(int i);
        int getVelocityRatio() const;
        void setProgramChange(int i);
        int getProgramChange() const;
        void setBusType(BusType);
        BusType getBusType() const;
        void setDeviceIndex(int i);
        int getDeviceIndex() const;
        std::shared_ptr<Event> getEvent(int i) const;
        void setName(const std::string &s);
        std::string getName();
        std::vector<std::shared_ptr<Event>> getEvents() const;

        int getNextTick();
        void playNext();
        bool isOn() const;
        bool isUsed() const;

        std::vector<std::shared_ptr<Event>> getEventRange(int startTick,
                                                          int endTick) const;

        // Do not call from audio thread
        void correctTimeRange(int startPos, int endPos, int stepLength,
                              int swingPercentage, int lowestNote,
                              int highestNote) const;

        // Do not call from audio thread
        void removeDoubles() const;

        void purge();

        std::pair<EventIndex, EventState>
            findRecordingNoteOnEventById(NoteEventId);

        std::pair<EventIndex, EventState>
            findRecordingNoteOnEventByNoteNumber(NoteNumber);

        std::shared_ptr<TrackEventStateManager> getEventStateManager();

    private:
        EventIndex playEventIndex{0};
        std::shared_ptr<TrackEventStateManager> eventStateManager;
        std::function<void(TrackEventMessage &&)> dispatch;
        BusType busType = BusType::DRUM1;
        std::string name;
        bool on{false};
        int velocityRatio = 0;
        int programChange = 0;
        int device = 0;
        TrackIndex trackIndex{0};
        bool used{false};

        NoteEventId nextNoteEventId{MinNoteEventId};

        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventState, moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOnEvents;
        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventState, moodycamel::ConcurrentQueueDefaultTraits>>
            queuedNoteOffEvents;

        Sequence *parent{nullptr};
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

        std::vector<EventState> bulkNoteOns;
        std::vector<EventState> bulkNoteOffs;

        void updateEventTick(const std::pair<EventIndex, EventState> &e,
                             int newTick) const;

        void processRealtimeQueuedEvents();
        int getCorrectedTickPos() const;
    };
} // namespace mpc::sequencer
