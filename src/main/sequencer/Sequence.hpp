#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string>

namespace mpc::sequencer
{
    class Bus;
    class Track;
    class TimeSignature;
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

        Sequence(std::function<std::string(int)> getDefaultTrackName,
                 std::function<int64_t()> getTickPosition,
                 std::function<std::shared_ptr<lcdgui::Screens>()> getScreens,
                 std::function<bool()> isRecordingModeMulti,
                 std::function<std::shared_ptr<Sequence>()> getActiveSequence,
                 std::function<int()> getAutoPunchMode,
                 std::function<std::shared_ptr<Bus>(int)> getSequencerBus,
                 std::function<bool()> isEraseButtonPressed,
                 std::function<bool(int programPadIndex,
                                    std::shared_ptr<sampler::Program>)>
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

    public:
        double getInitialTempo();
        void setInitialTempo(const double newInitialTempo);

        void setLoopStart(int l);
        int getLoopStart();
        void setLoopEnd(int l);
        int getLoopEnd();
        void setFirstLoopBarIndex(int i);
        int getFirstLoopBarIndex();
        void setLastLoopBarIndex(int i);
        int getLastLoopBarIndex();
        bool isLoopEnabled();
        void setName(std::string s);
        std::string getName();
        void setDeviceName(int i, std::string s);
        std::string getDeviceName(int i);
        void setLastBarIndex(int i);
        int getLastBarIndex();
        int getBarCount();
        void setLoopEnabled(bool b);
        std::shared_ptr<Track> getTrack(int i);
        void setUsed(bool b);
        bool isUsed();
        void init(int newLastBarIndex);
        void setTimeSignature(int firstBar, int tsLastBar, int num, int den);
        void setTimeSignature(int barIndex, int num, int den);
        std::vector<std::shared_ptr<Track>> getTracks();
        std::vector<std::string> &getDeviceNames();
        void setDeviceNames(std::vector<std::string> &sa);
        std::vector<std::shared_ptr<TempoChangeEvent>> getTempoChangeEvents();
        std::shared_ptr<TempoChangeEvent> addTempoChangeEvent(int tick);
        void removeTempoChangeEvent(int i);

        bool isTempoChangeOn();
        void setTempoChangeOn(bool b);
        int getLastTick();
        TimeSignature getTimeSignature();
        void purgeAllTracks();
        std::shared_ptr<Track> purgeTrack(int i);
        int getDenominator(int i);
        int getNumerator(int i);
        std::vector<int> &getBarLengthsInTicks();
        void setBarLengths(std::vector<int> &);
        void setNumeratorsAndDenominators(std::vector<int> &numerators,
                                          std::vector<int> &denominators);
        void deleteBars(int firstBar, int lBar);
        void insertBars(int numberOfBars, int afterBar);
        void moveTrack(int source, int destination);
        bool isLastLoopBarEnd();

        int getEventCount();
        void initLoop();
        std::vector<int> &getNumerators();
        std::vector<int> &getDenominators();

        int getFirstTickOfBar(int index);
        int getLastTickOfBar(int index);
        int getFirstTickOfBeat(int bar, int beat);

        void resetTrackEventIndices(int tick);

        std::shared_ptr<Track> getTempoChangeTrack();

        StartTime &getStartTime();

    private:
        StartTime startTime{0, 0, 0, 0, 0};
        std::atomic_bool tempoTrackIsInitialized{false};
        double initialTempo = 120.0;

        std::vector<std::shared_ptr<Track>> tracks;
        std::shared_ptr<Track> tempoChangeTrack;

        std::vector<std::string> deviceNames = std::vector<std::string>(33);

        std::vector<int> barLengthsInTicks = std::vector<int>(999);
        std::vector<int> numerators = std::vector<int>(999);
        std::vector<int> denominators = std::vector<int>(999);

        std::vector<int> oldBarLengthsInTicks = std::vector<int>(999);
        std::vector<int> oldNumerators = std::vector<int>(999);
        std::vector<int> oldDenominators = std::vector<int>(999);

        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<int()> getCurrentBarIndex;

        std::string name;
        bool loopEnabled = true;
        int lastBarIndex = -1;
        bool used = false;
        bool tempoChangeOn = true;
        int loopStart = 0;
        int loopEnd = 0;
        int firstLoopBarIndex = 0;
        int lastLoopBarIndex = 0;
        bool lastLoopBarEnd = true;
    };
} // namespace mpc::sequencer
