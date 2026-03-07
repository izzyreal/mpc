#pragma once

#include "IntTypes.hpp"
#include "MpcSpecs.hpp"
#include "engine/audio/server/AudioClient.hpp"
#include "engine/NoteRepeatProcessor.hpp"

#include <array>
#include <memory>
#include <functional>

namespace mpc::audiomidi
{
    class MidiOutput;
}

namespace mpc::performance
{
    class PerformanceManager;
}

namespace mpc::sequencer
{
    class Sequencer;
    class Clock;
    class Sequence;
    class MidiClockOutput;
} // namespace mpc::sequencer

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::lcdgui::screens
{
    class SequencerScreen;
    class UserScreen;
    class SyncScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;
    class TimingCorrectScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::engine
{
    class NoteRepeatProcessor;

    class SequencerPlaybackEngine final : public audio::server::AudioClient
    {
    public:
        explicit SequencerPlaybackEngine(
            performance::PerformanceManager *, EngineHost *,
            std::function<std::shared_ptr<audiomidi::MidiOutput>()>
                getMidiOutput,
            sequencer::Sequencer *, const std::shared_ptr<sequencer::Clock> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            std::function<bool()> isBouncing,
            const std::function<int()> &getSampleRate,
            const std::function<bool()> &isRecMainWithoutPlaying,
            const std::function<void(int velo, int frameOffset)> &playMetronome,
            std::function<std::shared_ptr<lcdgui::Screens>()> getScreens,
            const std::function<bool()> &isNoteRepeatLockedOrPressed,
            const std::shared_ptr<NoteRepeatProcessor> &,
            const std::function<bool()> &isAudioServerCurrentlyRunningOffline);

        void work(int nFrames) override;

        unsigned short getEventFrameOffset() const;

    private:
        performance::PerformanceManager *performanceManager;
        EngineHost *engineHost;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        sequencer::Sequencer *sequencer;
        std::shared_ptr<sequencer::Clock> clock;
        std::function<bool()> isBouncing;
        std::function<int()> getSampleRate;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> playMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;
        std::shared_ptr<NoteRepeatProcessor> noteRepeatProcessor;
        std::function<bool()> isAudioServerCurrentlyRunningOffline;
        lcdgui::screens::window::CountMetronomeScreen *countMetronomeScreen;
        lcdgui::screens::SequencerScreen *sequencerScreen;
        lcdgui::screens::UserScreen *userScreen;
        lcdgui::screens::window::TimingCorrectScreen *timingCorrectScreen;
        lcdgui::screens::SyncScreen *syncScreen;

        std::shared_ptr<sequencer::MidiClockOutput> midiClockOutput;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        struct SecondSequencePlaybackState
        {
            bool initialized = false;
            bool enabled = false;
            bool active = false;
            SequenceIndex sequenceIndex = NoSequenceIndex;
            Tick positionTicks = 0;
            std::array<EventIndex, Mpc2000XlSpecs::TOTAL_TRACK_COUNT>
                playEventIndices{};
        };

        mutable SecondSequencePlaybackState secondSequencePlaybackState;

        void triggerClickIfNeeded() const;

        void displayPunchRects() const;

        void stopCountingInIfRequired(SequenceIndex) const;

        std::shared_ptr<sequencer::Sequence> switchToNextSequence() const;

        bool processSongMode() const;

        bool processSeqLoopEnabled() const;

        bool processSeqLoopDisabled() const;

        void processNoteRepeat() const;

        void setTickPositionEffectiveImmediately(int newTickPos,
                                                 SequenceIndex) const;

        void stopSequencer() const;
        void resetSecondSequencePlaybackState() const;
        void initializeSecondSequencePlaybackState(Tick elapsedTicks) const;
        void syncSecondSequencePlayEventIndices(Tick targetTick) const;
        void playSecondSequenceTick() const;
        void advanceSecondSequencePlaybackState() const;
        Tick normalizeSecondSequenceStartTick(const sequencer::Sequence &,
                                              Tick elapsedTicks) const;
    };
} // namespace mpc::engine
