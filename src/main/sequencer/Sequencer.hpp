#pragma once

#include "BusType.hpp"
#include "IntTypes.hpp"

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::engine
{
    class Voice;
} // namespace mpc::engine

namespace mpc::hardware
{
    class Hardware;
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
    class Clock;
    class SequencerStateManager;
    class Transport;
    class Sequence;
    class Bus;
    class DrumBus;
    class Song;
    class Track;
    class TempoChangeEvent;
} // namespace mpc::sequencer

namespace mpc::performance
{
    class PerformanceManager;
}

namespace mpc::sequencer
{
    class Sequencer final : public std::enable_shared_from_this<Sequencer>
    {
    public:
        enum StopMode
        {
            AT_START_OF_BUFFER,
            AT_START_OF_TICK
        };

        Sequencer(const std::function<void()> &flushMidiNoteOffs,
                  const std::shared_ptr<Clock> &,
                  const std::shared_ptr<lcdgui::LayeredScreen> &,
                  const std::function<std::shared_ptr<lcdgui::Screens>()> &,
                  std::vector<std::shared_ptr<engine::Voice>> *,
                  const std::function<bool()> &isAudioServerRunning,
                  const std::shared_ptr<hardware::Hardware> &,
                  const std::function<bool()> &isBouncePrepared,
                  const std::function<void()> &startBouncing,
                  const std::function<void()> &stopBouncing,
                  const std::function<bool()> &isBouncing,
                  const std::function<bool()> &isEraseButtonPressed,
                  const std::shared_ptr<performance::PerformanceManager> &,
                  const std::shared_ptr<sampler::Sampler> &,
                  const std::shared_ptr<audiomidi::EventHandler> &,
                  const std::function<bool()> &isSixteenLevelsEnabled);

        ~Sequencer();

        static constexpr uint16_t TICKS_PER_QUARTER_NOTE = 96;
        static uint32_t quarterNotesToTicks(double quarterNotes);
        static double ticksToQuarterNotes(double ticks);

        void playTick(Tick) const;
        SequenceIndex getSelectedSequenceIndex() const;
        std::shared_ptr<Track> getSelectedTrack();
        std::shared_ptr<Sequence> createSeqInPlaceHolder();
        void clearPlaceHolder() const;
        void movePlaceHolderTo(int destIndex);
        std::shared_ptr<Sequence> getPlaceHolder();
        template <typename T> std::shared_ptr<T> getBus(BusType) const;
        std::shared_ptr<Bus> getBus(BusType) const;
        std::shared_ptr<DrumBus> getDrumBus(BusType) const;
        std::shared_ptr<DrumBus> getDrumBus(DrumBusIndex) const;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        const std::function<bool()> isBouncePrepared;
        const std::function<void()> startBouncing;
        const std::shared_ptr<hardware::Hardware> hardware;
        const std::function<bool()> isBouncing;
        const std::function<void()> stopBouncing;
        const std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<TempoChangeEvent> getCurrentTempoChangeEvent();
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();

        std::shared_ptr<Clock> clock;
        const std::function<void()> flushMidiNoteOffs;

    private:
        std::vector<std::shared_ptr<engine::Voice>> *voices;
        std::function<bool()> isAudioServerRunning;
        std::function<bool()> isEraseButtonPressed;
        std::weak_ptr<performance::PerformanceManager> performanceManager;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::function<bool()> isSixteenLevelsEnabled;

        std::shared_ptr<Transport> transport;
        std::vector<std::shared_ptr<Bus>> buses;

        std::vector<std::shared_ptr<Sequence>> sequences =
            std::vector<std::shared_ptr<Sequence>>(
                Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT);
        std::vector<std::shared_ptr<Song>> songs =
            std::vector<std::shared_ptr<Song>>(20);
        std::vector<uint64_t> taps{0, 0, 0, 0};

        std::shared_ptr<SequencerStateManager> stateManager;

        std::atomic<bool> secondSequenceEnabled{false};
        bool undoSeqAvailable = false;

        std::string defaultSequenceName;
        int timeDisplayStyle = 0;
        bool recordingModeMulti = false;
        int frameRate = 0;
        bool soloEnabled = false;

        uint64_t lastTap = 0;
        int tapIndex = 0;

        std::vector<std::string> defaultTrackNames;
        int selectedTrackIndex = 0;
        SequenceIndex nextSq{NoSequenceIndex};

        std::shared_ptr<Sequence>
        copySequence(const std::shared_ptr<Sequence> &source,
                     SequenceIndex destinationIndex);

        static void
        copySequenceParameters(const std::shared_ptr<Sequence> &source,
                               const std::shared_ptr<Sequence> &dest);
        static void copyTempoChangeEvents(const std::shared_ptr<Sequence> &src,
                                          const std::shared_ptr<Sequence> &dst);
        void copyTrack(const std::shared_ptr<Track> &src,
                              const std::shared_ptr<Track> &dest) const;

    public:
        void copyTrackParameters(const std::shared_ptr<Track> &source,
                                        const std::shared_ptr<Track> &dest) const;

        std::shared_ptr<SequencerStateManager> getStateManager() const;
        std::shared_ptr<Transport> getTransport();

        std::shared_ptr<Sequence> makeNewSequence(SequenceIndex sequenceIndex);

        void init();
        bool isSoloEnabled() const;
        void setSoloEnabled(bool b);
        std::shared_ptr<Sequence> getSequence(int i);
        std::string getDefaultSequenceName();
        void setDefaultSequenceName(const std::string &s);
        void setSelectedSequenceIndex(SequenceIndex,
                                      bool shouldSetPositionTo0) const;
        void setTimeDisplayStyle(int i);
        int getTimeDisplayStyle() const;
        void setRecordingModeMulti(bool b);
        bool isRecordingModeMulti() const;
        int getSelectedTrackIndex() const;

        void undoSeq();
        void setSequence(SequenceIndex, std::shared_ptr<Sequence>);
        void purgeAllSequences();
        void purgeSequence(int i);
        void copySequence(int source, int destination);
        void copySequenceParameters(int source, int dest) const;
        void copySong(int source, int dest) const;

        void copyTrack(int sourceTrackIndex, int destTrackIndex,
                       int sourceSequenceIndex,
                       int destSequenceIndex) const;
        std::vector<std::string> &getDefaultTrackNames();
        std::string getDefaultTrackName(int i);
        void setDefaultTrackName(const std::string &s, int i);
        std::shared_ptr<Sequence> getSelectedSequence();
        int getUsedSequenceCount() const;
        std::vector<std::shared_ptr<Sequence>> getUsedSequences() const;
        std::vector<int> getUsedSequenceIndexes() const;
        void goToPreviousEvent();
        void goToNextEvent();
        void goToPreviousStep();
        void goToNextStep();
        void tap();

        std::shared_ptr<Sequence> getCurrentlyPlayingSequence();
        void setSelectedTrackIndex(int i);
        SequenceIndex getCurrentlyPlayingSequenceIndex() const;
        SequenceIndex getNextSq() const;
        SequenceIndex getFirstUsedSeqDown(SequenceIndex from,
                                          bool unused = false) const;
        SequenceIndex getFirstUsedSeqUp(SequenceIndex from,
                                        bool unused = false) const;
        void setNextSq(SequenceIndex);
        void setNextSqPad(SequenceIndex);
        std::shared_ptr<Song> getSong(int i);
        std::shared_ptr<Song> getSelectedSong() const;
        void deleteSong(int i) const;
        bool isSongModeEnabled() const;
        SequenceIndex getSongSequenceIndex() const;
        bool isSecondSequenceEnabled() const;
        void setSecondSequenceEnabled(bool b);
        void flushTrackNoteCache() const;
        void storeSelectedSequenceInUndoPlaceHolder();
        void resetUndo();
        SongIndex getSelectedSongIndex() const;
        void setSelectedSongIndex(SongIndex) const;
        SongStepIndex getSelectedSongStepIndex() const;
        bool isSelectedSongStepIndexEndOfSong() const;
        void setSelectedSongStepIndex(SongStepIndex) const;
    };
} // namespace mpc::sequencer
