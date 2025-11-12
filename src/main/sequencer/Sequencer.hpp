#pragma once

#include "BusType.hpp"

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
    class SequencerPlaybackEngine;
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}

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
    class SequencerStateManager;
    class Transport;
    class Sequence;
    class Bus;
    class DrumBus;
    class Song;
    class Track;
    class TempoChangeEvent;
} // namespace mpc::sequencer

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::sequencer
{
    class Sequencer final
    {
    public:
        enum StopMode
        {
            AT_START_OF_BUFFER,
            AT_START_OF_TICK
        };

        Sequencer(const std::shared_ptr<lcdgui::LayeredScreen> &,
                  const std::function<std::shared_ptr<lcdgui::Screens>()> &,
                  std::vector<std::shared_ptr<engine::Voice>> *,
                  const std::function<bool()> &isAudioServerRunning,
                  const std::shared_ptr<hardware::Hardware> &,
                  const std::function<bool()> &isBouncePrepared,
                  const std::function<void()> &startBouncing,
                  const std::function<void()> &stopBouncing,
                  const std::function<bool()> &isBouncing,
                  const std::function<bool()> &isEraseButtonPressed,
                  const std::shared_ptr<eventregistry::EventRegistry> &,
                  const std::shared_ptr<sampler::Sampler> &,
                  const std::shared_ptr<audiomidi::EventHandler> &,
                  const std::function<bool()> &isSixteenLevelsEnabled,
                  const std::function<
                      std::shared_ptr<engine::SequencerPlaybackEngine>()> &);

        static constexpr uint16_t TICKS_PER_QUARTER_NOTE = 96;
        static uint32_t quarterNotesToTicks(double quarterNotes);
        static double ticksToQuarterNotes(uint32_t ticks);

        void playToTick(int targetTick) const;
        int getActiveSequenceIndex() const;
        std::shared_ptr<Track> getActiveTrack();
        std::shared_ptr<Sequence> createSeqInPlaceHolder();
        void clearPlaceHolder();
        void movePlaceHolderTo(int destIndex);
        std::shared_ptr<Sequence> getPlaceHolder();
        template <typename T> std::shared_ptr<T> getBus(int busIndex);
        std::shared_ptr<Bus> getBus(BusType);
        std::shared_ptr<DrumBus> getDrumBus(int drumBusIndex) const;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        const std::function<bool()> isBouncePrepared;
        const std::function<void()> startBouncing;
        const std::shared_ptr<hardware::Hardware> hardware;
        const std::function<bool()> isBouncing;
        const std::function<void()> stopBouncing;
        const std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<TempoChangeEvent> getCurrentTempoChangeEvent();

    private:
        std::vector<std::shared_ptr<engine::Voice>> *voices;
        std::function<bool()> isAudioServerRunning;
        std::function<bool()> isEraseButtonPressed;
        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::function<bool()> isSixteenLevelsEnabled;

        std::shared_ptr<SequencerStateManager> stateManager;
        std::function<std::shared_ptr<engine::SequencerPlaybackEngine>()>
            getSequencerPlaybackEngine;
        std::shared_ptr<Transport> transport;
        std::vector<std::shared_ptr<Bus>> buses;
        std::shared_ptr<Sequence> placeHolder;
        int activeSequenceIndex = 0;

        std::vector<std::shared_ptr<Sequence>> sequences =
            std::vector<std::shared_ptr<Sequence>>(99);
        std::vector<std::shared_ptr<Song>> songs =
            std::vector<std::shared_ptr<Song>>(20);
        std::vector<uint64_t> taps{0, 0, 0, 0};

        std::shared_ptr<Sequence> undoPlaceHolder;

        bool secondSequenceEnabled = false;
        bool undoSeqAvailable = false;

        std::string defaultSequenceName;
        int timeDisplayStyle = 0;
        bool recordingModeMulti = false;
        int frameRate = 0;
        bool soloEnabled = false;

        uint64_t lastTap = 0;
        int tapIndex = 0;

        std::vector<std::string> defaultTrackNames;
        int activeTrackIndex = 0;
        int nextSq = -1;

        std::shared_ptr<Sequence>
        copySequence(const std::shared_ptr<Sequence> &source);
        void
        copySequenceParameters(const std::shared_ptr<Sequence> &source,
                               const std::shared_ptr<Sequence> &dest) const;
        void copyTempoChangeEvents(const std::shared_ptr<Sequence> &src,
                                   const std::shared_ptr<Sequence> &dst) const;
        void copyTrack(const std::shared_ptr<Track> &src,
                       const std::shared_ptr<Track> &dest) const;

    public:
        static void copyTrackParameters(const std::shared_ptr<Track> &source,
                                        const std::shared_ptr<Track> &dest);

        std::shared_ptr<SequencerStateManager> getStateManager();
        std::shared_ptr<Transport> getTransport();

        std::shared_ptr<Sequence> makeNewSequence();

        void init();
        bool isSoloEnabled() const;
        void setSoloEnabled(bool b);
        std::shared_ptr<Sequence> getSequence(int i);
        std::string getDefaultSequenceName();
        void setDefaultSequenceName(const std::string &s);
        void setActiveSequenceIndex(int i, bool shouldSetPositionTo0);
        void setTimeDisplayStyle(int i);
        int getTimeDisplayStyle() const;
        void setRecordingModeMulti(bool b);
        bool isRecordingModeMulti() const;
        int getActiveTrackIndex() const;
        void trackUp();
        void trackDown();

        void undoSeq();
        void setSequence(int i, std::shared_ptr<Sequence> s);
        void purgeAllSequences();
        void purgeSequence(int i);
        void copySequence(int source, int destination);
        void copySequenceParameters(int source, int dest) const;
        void copySong(int source, int dest);

        void copyTrack(int sourceTrackIndex, int destinationTrackIndex,
                       int sourceSequenceIndex,
                       int destinationSequenceIndex) const;
        std::vector<std::string> &getDefaultTrackNames();
        std::string getDefaultTrackName(int i);
        void setDefaultTrackName(const std::string &s, int i);
        std::shared_ptr<Sequence> getActiveSequence();
        int getUsedSequenceCount() const;
        std::vector<std::shared_ptr<Sequence>> getUsedSequences() const;
        std::vector<int> getUsedSequenceIndexes() const;
        void goToPreviousEvent();
        void goToNextEvent();
        void goToPreviousStep();
        void goToNextStep();
        void tap();

        std::shared_ptr<Sequence> getCurrentlyPlayingSequence();
        void setActiveTrackIndex(int i);
        int getCurrentlyPlayingSequenceIndex() const;
        int getNextSq() const;
        int getFirstUsedSeqDown(int from, bool unused = false) const;
        int getFirstUsedSeqUp(int from, bool unused = false) const;
        void setNextSq(int i);
        void setNextSqPad(int i);
        std::shared_ptr<Song> getSong(int i);
        void deleteSong(int i);
        bool isSongModeEnabled() const;
        int getSongSequenceIndex() const;
        bool isSecondSequenceEnabled() const;
        void setSecondSequenceEnabled(bool b);
        void flushTrackNoteCache();
        void storeActiveSequenceInUndoPlaceHolder();
        void resetUndo();
    };
} // namespace mpc::sequencer
