#pragma once

#include "BusType.hpp"
#include "EventData.hpp"
#include "IntTypes.hpp"
#include "utils/SimpleAction.hpp"
#include "utils/SmallFn.hpp"

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <atomic>

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

        Sequencer(utils::PostToAudioThreadFn &,
                  const std::function<void()> &flushMidiNoteOffs,
                  const std::shared_ptr<Clock> &,
                  const std::shared_ptr<lcdgui::LayeredScreen> &,
                  const std::function<std::shared_ptr<lcdgui::Screens>()> &,
                  std::vector<std::shared_ptr<engine::Voice>> *,
                  const std::function<bool()> &isAudioServerRunning,
                  const std::shared_ptr<hardware::Hardware> &,
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
        template <typename T> std::shared_ptr<T> getBus(BusType) const;
        std::shared_ptr<Bus> getBus(BusType) const;
        std::shared_ptr<DrumBus> getDrumBus(BusType) const;
        std::shared_ptr<DrumBus> getDrumBus(DrumBusIndex) const;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        const std::weak_ptr<hardware::Hardware> hardware;
        const std::function<bool()> isBouncing;
        const std::function<void()> stopBouncing;
        const std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<TempoChangeEvent> getCurrentTempoChangeEvent();
        EventData *getCurrentTempoChangeEventData() const;
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();

        std::shared_ptr<Clock> clock;
        const std::function<void()> flushMidiNoteOffs;

    private:
        const utils::PostToAudioThreadFn &postToAudioThread;
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

        int timeDisplayStyle = 0;
        bool recordingModeMulti = false;
        int frameRate = 0;
        bool soloEnabled = false;

        uint64_t lastTap = 0;
        int tapIndex = 0;

        std::string defaultSequenceName = "Sequence";
        std::vector<std::string> defaultTrackNames;
        int selectedTrackIndex = 0;

        void makeNewSequence(SequenceIndex sequenceIndex);

    public:
        void makeNewSequence(std::shared_ptr<Sequence> &destination);

        void copyTrackParameters(const std::shared_ptr<Track> &source,
                                 const std::shared_ptr<Track> &dest) const;

        void copySequenceParameters(const std::shared_ptr<Sequence> &source,
                                    SequenceIndex destIndex) const;

        std::shared_ptr<SequencerStateManager> getStateManager() const;
        std::shared_ptr<Transport> getTransport();

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

        void undoSeq() const;
        void deleteAllSequences() const;
        void deleteSequence(int i) const;

        void copySequence(SequenceIndex sourceIndex, SequenceIndex destIndex,
                          utils::SimpleAction &&onComplete = {}) const;

        void copySong(int source, int dest) const;

        void copyTrack(int sourceTrackIndex, int destTrackIndex,
                       int sourceSequenceIndex, int destSequenceIndex) const;
        std::vector<std::string> &getDefaultTrackNames();
        std::string getDefaultTrackName(int i);
        void setDefaultTrackName(int i, const std::string &);
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

        /**
         * In the MAIN screen, the behaviour is like this:
         *
         * Turning the DATA wheel left selects the first used sequence below
         * the currently selected or next sequence. Turning it right selects the
         * first used sequence above the currently selected or next sequence.
         * Setting a next sequence can only be done while the sequencer is
         * playing. Once a next sequence is set, it's impossible to unset it. It
         * can only be changed to another next sequence, which may also be the
         * currently playing sequence. But the 'Next Sq:' widget that appears at
         * the bottom of the screen, overlaying the 'STEP' and 'EDIT' function
         * key labels, remains visible for as long as the sequencer has not
         * switched to the next sequence (or the sequencer has stopped playing).
         * In this screen, setting a next sequence from scratch means that the
         * user goes from having no next sequence set, to some next sequence
         * set. This type of from-scratch-ness is trivially determined by
         * checking if the current state of 'nextSequence' equals
         * 'NoNextSequenceIndex', which we do inside `setNextSq`. If this
         * evaluates to true, we're making a from-scratch selection, which means
         * that the current next sequence is the point of reference, rather than
         * the currently selected sequence.
         *
         * In the NEXT SEQ screen, the 'from scratch' state is attained in a
         * different way. If the user enters the NEXT SEQ screen, or they change
         * the currently selected sequence while the sequencer is not playing,
         * or the sequencer has switched to the next sequence, we enter the 'from
         * scratch' state in this screen. A next sequence can be set via the
         * 'Sq:' field, or via the 'Next Sq:' field. Setting it via the
         * 'Next Sq:' field never yields a 'from scratch' state, even though the
         * user may select no next sequence via this field by turning the DATA
         * wheel left until the 'Next Sq:' field is empty.
         *
         * For this reason, the `Sequencer::setNextSq` API has an optional bool.
         * The MAIN screen invokes it while passing `std::nullopt` for the
         * `fromScratch` parameter, so the implementation relies on the
         * `nextSequence == NoNextSequenceIndex` comparison mentioned above. The
         * NEXT SEQ screen invokes it while passing a non-empty optional,
         * containing the from-scratch state that is owned and maintained by
         * this screen itself. Note that it only does that when the user sets
         * the next sequence via the 'Next Sq:' field. Setting it via the 'Sq:'
         * field in the NEXT SEQ screen is 100% the same as setting it via the
         * 'Sq:' field in the MAIN screen, as is resetting the next sequence to
         * `NoNextSequenceIndex` via the 'CLEAR' function.
         */
        void setNextSq(SequenceIndex, std::optional<bool> fromScratch) const;

        void clearNextSq() const;

        void setNextSqPad(SequenceIndex) const;
        std::shared_ptr<Song> getSong(int i);
        std::shared_ptr<Song> getSelectedSong() const;
        void deleteSong(int i) const;
        bool isSongModeEnabled() const;
        SequenceIndex getSongSequenceIndex() const;
        bool isSecondSequenceEnabled() const;
        void setSecondSequenceEnabled(bool b);
        void flushTrackNoteCache() const;
        void copySelectedSequenceToUndoSequence() const;
        void resetUndo() const;
        SongIndex getSelectedSongIndex() const;
        void setSelectedSongIndex(SongIndex) const;
        SongStepIndex getSelectedSongStepIndex() const;
        bool isSelectedSongStepIndexEndOfSong() const;
        void setSelectedSongStepIndex(SongStepIndex) const;
    };
} // namespace mpc::sequencer
