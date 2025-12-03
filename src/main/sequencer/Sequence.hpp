#pragma once

#include "sequencer/BusType.hpp"
#include "IntTypes.hpp"
#include "SequencerMessage.hpp"
#include "SequencerStateView.hpp"

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

        Sequence(SequenceIndex, std::shared_ptr<SequencerStateManager> manager,
                 const std::function<std::shared_ptr<SequenceStateView>()>
                     &getSnapshot,
                 const std::function<void(SequenceMessage &&)> &dispatch,
                 std::function<std::string(int)> getDefaultTrackName,
                 std::function<int64_t()> getTickPosition,
                 std::function<std::shared_ptr<lcdgui::Screens>()> getScreens,
                 std::function<bool()> isRecordingModeMulti,
                 std::function<std::shared_ptr<Sequence>()> getActiveSequence,
                 std::function<int()> getAutoPunchMode,
                 std::function<std::shared_ptr<Bus>(BusType)> getBus,
                 std::function<bool()> isEraseButtonPressed,
                 std::function<bool(ProgramPadIndex, ProgramIndex)>
                     isProgramPadPressed,
                 std::shared_ptr<sampler::Sampler> sampler,
                 std::shared_ptr<audiomidi::EventHandler> eventHandler,
                 std::function<bool()> isSixteenLevelsEnabled,
                 std::function<int()> getActiveTrackIndex,
                 std::function<bool()> isRecording,
                 std::function<bool()> isOverdubbing,
                 std::function<bool()> isPunchEnabled,
                 std::function<int64_t()> getPunchInTime,
                 std::function<int64_t()> getPunchOutTime,
                 std::function<bool()> isSoloEnabled,
                 std::function<int()> getCurrentBarIndex);

        ~Sequence();

        SequenceIndex getSequenceIndex() const;

        // Replace all bar lengths in bulk.
        // Unlike setBarLength and setTimeSignature, this method does not
        // modify tick positions of events. It just replaces the bar lengths,
        // nothing more.
        void setBarLengths(const std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT>
                               &barLengths) const;

        double getInitialTempo() const;
        void setInitialTempo(double initialTempo) const;

        Tick getLoopStartTick() const;
        Tick getLoopEndTick() const;
        void setFirstLoopBarIndex(BarIndex) const;
        BarIndex getFirstLoopBarIndex() const;
        void setLastLoopBarIndex(BarIndex) const;
        BarIndex getLastLoopBarIndex() const;
        bool isLoopEnabled() const;
        void setName(const std::string &s);
        std::string getName();
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

        void syncTrackEventIndices(int tick) const;

        // Replace all time signatures in bulk.
        // Unlike setBarLength and setTimeSignature, this method does not
        // modify tick positions of events. And unlike setTimeSignature, it
        // does not modify any bar lengths. It just replaces the time
        // signatures, nothing more.
        void setTimeSignatures(
            const std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> &)
            const;

        std::vector<std::shared_ptr<Track>> getTracks();
        std::vector<std::string> &getDeviceNames();
        void setDeviceNames(const std::vector<std::string> &sa);
        std::vector<std::shared_ptr<TempoChangeEvent>>
        getTempoChangeEvents() const;
        void addTempoChangeEvent(int tick, int amount) const;
        void removeTempoChangeEvent(int i) const;

        bool isTempoChangeOn() const;
        void setTempoChangeOn(bool b) const;
        int getLastTick() const;
        TimeSignature getTimeSignature(int barIndex) const;
        TimeSignature getTimeSignatureFromTickPos(Tick) const;
        void purgeAllTracks();
        std::shared_ptr<Track> purgeTrack(int i);
        int getDenominator(int i) const;
        int getNumerator(int i) const;
        void deleteBars(int firstBar, int lastBarToDelete) const;
        void insertBars(
            int barCount, BarIndex afterBar,
            std::function<void()> onComplete2 = [] {}) const;
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

        const std::function<std::shared_ptr<SequenceStateView>()> getSnapshot;

    private:
        SequenceIndex sequenceIndex;
        std::shared_ptr<SequencerStateManager> manager;
        const std::function<void(SequenceMessage &&)> dispatch;
        StartTime startTime{0, 0, 0, 0, 0};

        std::vector<std::shared_ptr<Track>> tracks;

        std::vector<std::string> deviceNames = std::vector<std::string>(33);

        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<int()> getCurrentBarIndex;

        std::string name;
    };
} // namespace mpc::sequencer
