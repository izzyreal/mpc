#pragma once

#include "IntTypes.hpp"

#include "sequencer/BusType.hpp"
#include "sequencer/TrackStateView.hpp"
#include "sequencer/TrackMessage.hpp"
#include "sequencer/EventData.hpp"

#include "utils/SimpleAction.hpp"
#include "utils/SmallFn.hpp"

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

    using GetTrackSnapshotFn = utils::SmallFn<8, TrackStateView(TrackIndex)>;

    class Track
    {
    public:
        Track(
            const utils::PostToUiThreadFn &,
            const std::function<std::string(int)> &getDefaultTrackName,
            const std::shared_ptr<SequencerStateManager> &,
            GetTrackSnapshotFn &&getSnapshot,
            const std::function<void(TrackMessage &&)> &dispatch,
            int trackIndex, Sequence *parent,
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

        void timingCorrect(const SequenceStateView &, int fromBar, int toBar,
                           EventData *, Tick eventTick, int stepLength,
                           int swingPercentage) const;

        static int timingCorrectTick(const SequenceStateView &, int fromBar,
                                     int toBar, int tick, int stepLength,
                                     int swingPercentage);

        void shiftTiming(EventData *, Tick eventTick, bool later, int amount,
                         int lastTick) const;

        void setTrackIndex(TrackIndex i);
        TrackIndex getIndex() const;
        void setOn(bool b, bool updateUsedness = true) const;

        void acquireAndInsertEvent(const EventData &,
                                   utils::SimpleAction &&onComplete = {}) const;

        EventData *recordNoteEventNonLive(int tick, NoteNumber, Velocity,
                                          int64_t metronomeOnlyTick = 0) const;

        void finalizeNoteEventNonLive(EventData *, Duration) const;

        void removeEvent(EventData *) const;
        void removeEvent(const std::shared_ptr<EventRef> &event) const;
        void removeEvents() const;
        void setVelocityRatio(int i, bool updateUsedness = true) const;
        int getVelocityRatio() const;
        void setProgramChange(int i, bool updateUsedness = true) const;
        int getProgramChange() const;
        void setBusType(BusType, bool updateUsedness = true) const;
        BusType getBusType() const;
        void setDeviceIndex(int i, bool updateUsedness = true) const;
        int getDeviceIndex() const;
        std::shared_ptr<EventRef> getEvent(int i) const;
        void setName(const std::string &s) const;
        std::string getName() const;
        std::vector<std::shared_ptr<EventRef>> getEvents() const;
        std::vector<EventData> getEventStates() const;
        std::vector<EventData *> getEventHandles() const;

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

        void printEvents() const;

        SequenceIndex getSequenceIndex() const;

        void setUsedIfCurrentlyUnused(
            utils::SimpleAction &&onCompleteNameSetting = {}) const;

        bool isTransmitProgramChangesEnabled() const;

        void setTransmitProgramChangesEnabled(bool,
                                              bool updateUsedness = true) const;

        int64_t getVersion() const;

    private:
        const utils::PostToUiThreadFn &postToUiThread;
        const std::function<std::string(int)> getDefaultTrackName;
        Sequence *parent{nullptr};
        std::shared_ptr<SequencerStateManager> manager;
        GetTrackSnapshotFn getSnapshot;
        std::function<void(TrackMessage &&)> dispatch;

        TrackIndex trackIndex{0};

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

        void insertAcquiredEvent(EventData *event,
                                 utils::SimpleAction &&onComplete = {}) const;
    };
} // namespace mpc::sequencer
