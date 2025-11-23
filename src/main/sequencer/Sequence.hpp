#pragma once

#include "sequencer/BusType.hpp"
#include "IntTypes.hpp"

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string>

namespace mpc::sequencer
{
    class Bus;
    class Track;
    struct TimeSignature;
    class TempoChangeEvent;
    class Sequencer;
    class SequenceStateManager;
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

        Sequence(std::function<std::string(int)> getDefaultTrackName,
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

        void setBarLengths(const std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT>
                               &barLengths) const;
        double getInitialTempo() const;
        void setInitialTempo(double newInitialTempo);

        void setLoopStart(int l);
        int getLoopStart() const;
        void setLoopEnd(int l);
        int getLoopEnd() const;
        void setFirstLoopBarIndex(int i);
        int getFirstLoopBarIndex() const;
        void setLastLoopBarIndex(int i);
        int getLastLoopBarIndex() const;
        bool isLoopEnabled() const;
        void setName(const std::string &s);
        std::string getName();
        void setDeviceName(int i, const std::string &s);
        std::string getDeviceName(int i);
        void setLastBarIndex(int i);
        int getLastBarIndex() const;
        int getBarCount() const;
        void setLoopEnabled(bool b);
        std::shared_ptr<Track> getTrack(int i);
        void setUsed(bool b);
        bool isUsed() const;
        void init(int newLastBarIndex);
        void setTimeSignature(int firstBar, int tsLastBar, int num, int den);
        void setTimeSignature(int barIndex, int num, int den);
        std::vector<std::shared_ptr<Track>> getTracks();
        std::vector<std::string> &getDeviceNames();
        void setDeviceNames(const std::vector<std::string> &sa);
        std::vector<std::shared_ptr<TempoChangeEvent>>
        getTempoChangeEvents() const;
        void addTempoChangeEvent(int tick, int amount) const;
        void removeTempoChangeEvent(int i) const;

        bool isTempoChangeOn() const;
        void setTempoChangeOn(bool b);
        int getLastTick() const;
        TimeSignature getTimeSignature() const;
        void purgeAllTracks();
        std::shared_ptr<Track> purgeTrack(int i);
        int getDenominator(int i) const;
        int getNumerator(int i) const;
        void deleteBars(int firstBar, int lastBarToDelete);
        void insertBars(int barCount, int afterBar);
        void moveTrack(int source, int destination);
        bool isLastLoopBarEnd() const;

        int getEventCount() const;
        void initLoop();

        int getFirstTickOfBar(int index) const;
        int getLastTickOfBar(int index) const;
        int getFirstTickOfBeat(int bar, int beat) const;

        void syncTrackEventIndices(int tick) const;

        std::shared_ptr<Track> getTempoChangeTrack();

        StartTime &getStartTime();

        std::shared_ptr<SequenceStateManager> getStateManager();

    private:
        std::shared_ptr<SequenceStateManager> stateManager;
        StartTime startTime{0, 0, 0, 0, 0};
        std::atomic_bool tempoTrackIsInitialized{false};
        double initialTempo = 120.0;

        std::vector<std::shared_ptr<Track>> tracks;
        std::shared_ptr<Track> tempoChangeTrack;

        std::vector<std::string> deviceNames = std::vector<std::string>(33);

        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<int()> getCurrentBarIndex;

        std::string name;
        bool loopEnabled = true;
        std::atomic<int> lastBarIndex{-1};
        bool used = false;
        bool tempoChangeOn = true;
        int loopStart = 0;
        int loopEnd = 0;
        int firstLoopBarIndex = 0;
        int lastLoopBarIndex = 0;
        bool lastLoopBarEnd = true;
    };
} // namespace mpc::sequencer
