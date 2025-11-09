#pragma once

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
    class Sequence;
    class Bus;
    class DrumBus;
    class Song;
    class Track;
    class TempoChangeEvent;
    class FrameSeq;
    class Clock;
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

        Sequencer(
            std::shared_ptr<lcdgui::LayeredScreen>,
            std::function<std::shared_ptr<lcdgui::Screens>()>,
            std::vector<std::shared_ptr<engine::Voice>> *,
            const std::function<bool()> &isAudioServerRunning,
            const std::shared_ptr<hardware::Hardware> &,
            const std::function<bool()> &isBouncePrepared,
            const std::function<void()> &startBouncing,
            const std::function<void()> &stopBouncing,
            std::function<bool()> isBouncing,
            const std::function<bool()> &isEraseButtonPressed,
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<sampler::Sampler>,
            const std::shared_ptr<audiomidi::EventHandler> &,
            std::function<bool()> isSixteenLevelsEnabled,
            std::shared_ptr<Clock> clock, std::function<int()> getSampleRate,
            std::function<bool()> isRecMainWithoutPlaying,
            std::function<bool()> isNoteRepeatLockedOrPressed,
            std::function<std::shared_ptr<engine::audio::mixer::AudioMixer>()>
                getAudioMixer,
            std::function<bool()> isFullLevelEnabled,
            std::function<std::vector<engine::MixerInterconnection *> &()>
                getMixerInterconnections);

        static constexpr uint16_t TICKS_PER_QUARTER_NOTE = 96;
        static uint32_t quarterNotesToTicks(const double quarterNotes);
        static double ticksToQuarterNotes(const uint32_t ticks);

        int countInStartPos = -1;
        int countInEndPos = -1;
        void playToTick(int targetTick) const;
        int getActiveSequenceIndex() const;
        std::shared_ptr<Track> getActiveTrack();
        std::shared_ptr<Sequence> createSeqInPlaceHolder();
        void clearPlaceHolder();
        void movePlaceHolderTo(int destIndex);
        std::shared_ptr<Sequence> getPlaceHolder();

        void setPosition(const double positionQuarterNotes);
        void setPositionWithinSong(const double positionQuarterNotes);
        void bumpPositionByTicks(const uint8_t ticks);

        template <typename T> std::shared_ptr<T> getBus(const int busIndex);
        std::shared_ptr<DrumBus> getDrumBus(const int drumBusIndex) const;
        std::shared_ptr<mpc::sequencer::FrameSeq> getFrameSequencer();

    private:
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::vector<std::shared_ptr<engine::Voice>> *voices;
        std::shared_ptr<FrameSeq> frameSequencer;
        std::function<bool()> isAudioServerRunning;
        std::shared_ptr<hardware::Hardware> hardware;
        std::function<bool()> isBouncePrepared;
        std::function<void()> startBouncing;
        std::function<void()> stopBouncing;
        std::function<bool()> isBouncing;
        std::function<bool()> isEraseButtonPressed;
        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::function<bool()> isSixteenLevelsEnabled;

        std::shared_ptr<SequencerStateManager> stateManager;
        std::vector<std::shared_ptr<Bus>> buses;
        bool playing = false;
        int lastNotifiedBar = -1;
        int lastNotifiedBeat = -1;
        int lastNotifiedClock = -1;
        std::shared_ptr<Sequence> placeHolder;
        bool metronomeOnly = false;
        int activeSequenceIndex = 0;
        int currentlyPlayingSequenceIndex = 0;
        int playedStepRepetitions = 0; // Part of SONG mode
        bool endOfSong = false;

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
        bool countEnabled = false;
        bool soloEnabled = false;
        bool tempoSourceSequenceEnabled = false;

        bool countingIn = false;
        uint64_t lastTap = 0;
        int tapIndex = 0;

        std::vector<std::string> defaultTrackNames;
        bool overdubbing = false;
        bool recording = false;
        int activeTrackIndex = 0;
        double tempo = 120.0;
        int nextSq = -1;

        // Punch properties
        bool punchEnabled = false;
        int autoPunchMode = 0;
        int punchInTime = 0;
        int punchOutTime = 0;

        std::shared_ptr<TempoChangeEvent> getCurrentTempoChangeEvent();
        void play(bool fromStart);
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

        std::shared_ptr<Sequence> makeNewSequence();

        void init();
        void setTempo(double newTempo);
        double getTempo();
        bool isTempoSourceSequenceEnabled() const;
        void setTempoSourceSequence(bool b);
        bool isRecordingOrOverdubbing() const;
        bool isRecording() const;
        bool isSoloEnabled() const;
        void setSoloEnabled(bool b);
        std::shared_ptr<Sequence> getSequence(int i);
        std::string getDefaultSequenceName();
        void setDefaultSequenceName(const std::string &s);
        void setActiveSequenceIndex(int i);
        bool isCountEnabled() const;
        void setCountEnabled(bool b);
        void setTimeDisplayStyle(int i);
        int getTimeDisplayStyle() const;
        void setRecordingModeMulti(bool b);
        bool isRecordingModeMulti() const;
        int getActiveTrackIndex() const;
        void trackUp();
        void trackDown();
        bool isPlaying() const;
        void setEndOfSong(bool b);

        // Repititions of steps in SONG mode (not step editor)
        int getPlayedStepRepetitions() const;
        void incrementPlayedStepRepetitions();
        void resetPlayedStepRepetitions();

    public:
        void undoSeq();
        void playFromStart();
        void play();
        void rec();
        void recFromStart();
        void overdub();
        void switchRecordToOverdub();
        void overdubFromStart();
        void stop();
        void stop(const StopMode);
        bool isCountingIn() const;
        void setCountingIn(bool b);
        void setSequence(int i, std::shared_ptr<Sequence> s);
        void purgeAllSequences();
        void purgeSequence(int i);
        void copySequence(int sq0, int sq1);
        void copySequenceParameters(const int source, const int dest) const;
        void copySong(const int source, const int dest);

    public:
        void copyTrack(int sourceTrackIndex, int destinationTrackIndex,
                       int sourceSequenceIndex,
                       int destinationSequenceIndex) const;
        std::vector<std::string> &getDefaultTrackNames();
        std::string getDefaultTrackName(int i);
        void setDefaultTrackName(const std::string &s, int i);
        int getCurrentBarIndex();
        int getCurrentBeatIndex();
        int getCurrentClockNumber();
        void setBar(int i);
        void setBeat(int i);
        void setClock(int i);
        int getLoopEnd();
        std::shared_ptr<Sequence> getActiveSequence();
        int getUsedSequenceCount() const;
        std::vector<std::shared_ptr<Sequence>> getUsedSequences() const;
        std::vector<int> getUsedSequenceIndexes() const;
        void goToPreviousEvent();
        void goToNextEvent();
        void goToPreviousStep();
        void goToNextStep();
        void tap();

        // Applies wrap-around within the active or playing sequence
        void move(const double positionQuarterNotes);

        // Applies wrap-around within the current song.
        // If the current screen is not the Song screen, it results in a no-op.
        void moveWithinSong(const double positionQuarterNotes);

        int getTickPosition() const;
        std::shared_ptr<Sequence> getCurrentlyPlayingSequence();
        void setActiveTrackIndex(int i);
        int getCurrentlyPlayingSequenceIndex() const;
        void setCurrentlyPlayingSequenceIndex(int i);
        int getNextSq() const;
        int getFirstUsedSeqDown(int from, bool unused = false) const;
        int getFirstUsedSeqUp(int from, bool unused = false) const;
        void setNextSq(int i);
        void setNextSqPad(int i);
        std::shared_ptr<Song> getSong(int i);
        void deleteSong(int i);
        bool isSongModeEnabled() const;
        void setSongModeEnabled(bool b);
        int getSongSequenceIndex() const;
        bool isSecondSequenceEnabled() const;
        void setSecondSequenceEnabled(bool b);
        void flushTrackNoteCache();
        void storeActiveSequenceInUndoPlaceHolder();
        void resetUndo();
        bool isOverdubbing() const;
        double getPlayStartPositionQuarterNotes() const;

        void setRecording(bool b);
        void setOverdubbing(bool b);
        void playMetronomeTrack();
        void stopMetronomeTrack();

        // Punch property getters and setters
        bool isPunchEnabled() const;
        void setPunchEnabled(bool enabled);
        int getAutoPunchMode() const;
        void setAutoPunchMode(int mode);
        int getPunchInTime() const;
        void setPunchInTime(int time);
        int getPunchOutTime() const;
        void setPunchOutTime(int time);
    };
} // namespace mpc::sequencer
