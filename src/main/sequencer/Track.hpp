#pragma once

#include "IntTypes.hpp"

#include "sequencer/BusType.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/TrackMessage.hpp"
#include "sequencer/EventData.hpp"

#include "utils/SimpleAction.hpp"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <cstdint>

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
    class SequencerStateManager;
    class Sequence;
    class EventRef;
    class NoteOnEvent;
    class Bus;
    class SequenceStateView;

    class Track
    {
    public:
        Track(
            const std::shared_ptr<SequencerStateManager> &,
            const std::function<std::shared_ptr<TrackStateView>(TrackIndex)>
                &getSnapshot,
            const std::function<void(TrackMessage &&)> &dispatch,
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

        ~Track();

        // Allocates! Don't invoke on audio thread
        std::vector<std::shared_ptr<NoteOnEvent>> getNoteEvents() const;

        void timingCorrect(const SequenceStateView &, int fromBar, int toBar, EventData *, Tick eventTick,
                           int stepLength, int swingPercentage) const;

        static int timingCorrectTick(const SequenceStateView &, int fromBar, int toBar, int tick, int stepLength,
                              int swingPercentage);

        void shiftTiming(EventData *, Tick eventTick, bool later, int amount,
                         int lastTick) const;

        std::string getActualName();

        void setTrackIndex(TrackIndex i);
        TrackIndex getIndex() const;
        void setUsed(bool b);
        void setOn(bool b) const;

        void insertAcquiredEvent(
            EventData *event,
            const utils::SimpleAction &onComplete = utils::SimpleAction([] {}));

        void acquireAndInsertEvent(
            const EventData &,
            const utils::SimpleAction &onComplete = utils::SimpleAction([] {}));

        EventData *recordNoteEventNonLive(int tick, NoteNumber, Velocity,
                                          int64_t metronomeOnlyTick = 0);

        void finalizeNoteEventNonLive(EventData *, Duration) const;

        void removeEvent(EventData *) const;
        void removeEvent(const std::shared_ptr<EventRef> &event) const;
        void removeEvents() const;
        void setVelocityRatio(int i) const;
        int getVelocityRatio() const;
        void setProgramChange(int i) const;
        int getProgramChange() const;
        void setBusType(BusType) const;
        BusType getBusType() const;
        void setDeviceIndex(int i) const;
        int getDeviceIndex() const;
        std::shared_ptr<EventRef> getEvent(int i) const;
        void setName(const std::string &s);
        std::string getName();
        std::vector<std::shared_ptr<EventRef>> getEvents() const;
        std::vector<EventData> getEventStates() const;

        int getNextTick() const;
        bool shouldRemovePlayIndexEventDueToRecording() const;
        bool shouldRemovePlayIndexEventDueToErasePressed() const;
        void playNext() const;

        bool isOn() const;
        bool isUsed() const;

        // Returns the events between startTick (inclusive) and endTick
        // (exclusive)
        std::vector<std::shared_ptr<EventRef>> getEventRange(int startTick,
                                                             int endTick) const;

        // Do not call from audio thread
        void correctTimeRange(int startPos, int endPos, int stepLength,
                              int swingPercentage, int lowestNote,
                              int highestNote) const;

        // Do not call from audio thread
        void removeDoubles() const;

        void purge();

        void printEvents() const;

        void setEventStates(const std::vector<EventData> &eventStates) const;

        SequenceIndex getSequenceIndex() const;

    private:
        Sequence *parent{nullptr};
        std::shared_ptr<SequencerStateManager> manager;
        std::function<std::shared_ptr<TrackStateView>(TrackIndex)> getSnapshot;
        std::function<void(TrackMessage &&)> dispatch;
        std::string name;
        TrackIndex trackIndex{0};

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

        void updateEventTick(EventData *, int newTick) const;

        void initializeTrackName();
    };
} // namespace mpc::sequencer
