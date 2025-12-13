#pragma once

#include "IntTypes.hpp"

#include "sequencer/BusType.hpp"
#include "sequencer/SequenceMessage.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "utils/SimpleAction.hpp"
#include "utils/SmallFn.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace mpc::sequencer
{
    class SequencerStateManager;
    class Bus;
    class Track;
    struct TimeSignature;
    class TempoChangeEvent;
    class Sequencer;
} // namespace mpc::sequencer

namespace mpc::lcdgui
{
    class Screens;
}

namespace mpc::sampler
{
    class Program;
    class Sampler;
} // namespace mpc::sampler

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::sequencer
{
    using GetSequenceSnapshotFn =
        utils::SmallFn<8, SequenceStateView(SequenceIndex)>;

    class Sequence final
    {
    public:
        struct StartTime
        {
            uint8_t hours;
            uint8_t minutes;
            uint8_t seconds;
            uint8_t frames;
            uint8_t frameDecimals;
        };

        Sequence(
            const utils::PostToUiThreadFn &,
            std::shared_ptr<SequencerStateManager> manager,
            GetSequenceSnapshotFn &getSnapshot,
            const std::function<void(SequenceMessage &&)> &dispatch,
            const std::function<std::string()> &getDefaultSequenceName,
            const std::function<std::string(int)> &getDefaultTrackName,
            const std::function<int64_t()> &getTickPosition,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &getScreens,
            const std::function<bool()> &isRecordingModeMulti,
            const std::function<std::shared_ptr<Sequence>()> &getActiveSequence,
            const std::function<int()> &getAutoPunchMode,
            const std::function<std::shared_ptr<Bus>(BusType)> &getBus,
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
            const std::function<bool()> &isSoloEnabled,
            const std::function<int()> &getCurrentBarIndex);

        ~Sequence();

        SequenceIndex getSequenceIndex() const;

        double getInitialTempo() const;
        void setInitialTempo(double initialTempo) const;

        void setSequenceIndex(SequenceIndex);

        Tick getLoopStartTick() const;
        Tick getLoopEndTick() const;
        void setFirstLoopBarIndex(BarIndex) const;
        BarIndex getFirstLoopBarIndex() const;
        void setLastLoopBarIndex(BarIndex) const;
        BarIndex getLastLoopBarIndex() const;
        bool isLoopEnabled() const;
        void setName(const std::string &s) const;
        std::string getName() const;
        void setDeviceName(int i, const std::string &s);
        std::string getDeviceName(int i);
        void setLastBarIndex(int i) const;
        int getLastBarIndex() const;
        int getBarCount() const;
        void setLoopEnabled(bool b) const;
        std::shared_ptr<Track> getTrack(int i);
        void setUsed(bool b) const;
        bool isUsed() const;
        void init(int newLastBarIndex);
        BarIndex getBarIndexForPositionQN(PositionQuarterNotes) const;
        BarIndex getBarIndexForPositionTicks(Tick) const;

        void setTimeSignature(int firstBar, int tsLastBar, int num,
                              int den) const;
        void setTimeSignature(int barIndex, int num, int den) const;

        void syncTrackEventIndices() const;

        std::vector<std::shared_ptr<Track>> &getTracks();
        std::vector<std::string> &getDeviceNames();
        void setDeviceNames(const std::vector<std::string> &sa);
        std::vector<std::shared_ptr<TempoChangeEvent>>
        getTempoChangeEvents() const;
        void addTempoChangeEvent(int tick, int amount) const;
        void removeTempoChangeEvent(int i) const;

        bool isTempoChangeOn() const;
        void setTempoChangeOn(bool b) const;
        int getLastTick() const;
        TimeSignature getTimeSignatureFromBarIndex(int barIndex) const;
        TimeSignature getTimeSignatureFromTickPos(Tick) const;
        int getDenominator(int i) const;
        int getNumerator(int i) const;

        // firstBar and lastBar are inclusive
        void deleteBars(int firstBar, int lastBar) const;

        // Deletes a track and sets the following values according to the
        // USER screen defaults:
        // - Program change (0 - 128)
        // - Velocity ratio (0 - 200)
        // - Bus type (MIDI, DRUM1...DRUM4)
        // - MIDI device index i.e. MIDI output port and channel (0 - 32)
        // Other track properties have fixed defaults that they are set to:
        // - On/Off status is set to On
        // - Name is set to "(Unused)"
        // - Usedness is set to false
        // - All events are removed
        void deleteTrack(TrackIndex) const;

        // Deletes all 64 non-meta tracks with the same behaviour as
        // `deleteTrack`. Note that the tempo track at index 64 (the 65th
        // track) has to be treated separatedly.
        void deleteAllTracks() const;

        void insertBars(int barCount, BarIndex afterBar,
                        utils::SimpleAction &&nextAction = {}) const;

        void moveTrack(int source, int destination);

        int getEventCount() const;

        int getFirstTickOfBar(int barIndex) const;
        int getLastTickOfBar(int barIndex) const;
        int getFirstTickOfBeat(int barIndex, int beat) const;

        std::shared_ptr<Track> getTempoChangeTrack() const;

        StartTime &getStartTime();

        Tick getBarLength(int barIndex) const;

        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> getBarLengths() const;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT>
        getTimeSignatures() const;

        GetSequenceSnapshotFn getSnapshot;

    private:
        std::function<std::string()> getDefaultSequenceName;
        SequenceIndex sequenceIndex{NoSequenceIndex};
        std::shared_ptr<SequencerStateManager> manager;
        const std::function<void(SequenceMessage &&)> dispatch;
        StartTime startTime{0, 0, 0, 0, 0};

        std::vector<std::shared_ptr<Track>> tracks;

        std::vector<std::string> defaultDeviceNames =
            std::vector<std::string>(32);

        std::vector<std::string> deviceNames = std::vector<std::string>(32);

        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<int()> getCurrentBarIndex;
    };
} // namespace mpc::sequencer
