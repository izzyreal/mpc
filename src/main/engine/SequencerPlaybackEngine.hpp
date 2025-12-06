#pragma once

#include "IntTypes.hpp"
#include "engine/audio/server/AudioClient.hpp"
#include "engine/NoteRepeatProcessor.hpp"

#include <memory>
#include <functional>

namespace mpc::audiomidi
{
    class MidiOutput;
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

namespace mpc::engine
{
    class NoteRepeatProcessor;

    class SequencerPlaybackEngine final : public audio::server::AudioClient
    {
    public:
        explicit SequencerPlaybackEngine(
            EngineHost *,
            std::function<std::shared_ptr<audiomidi::MidiOutput>()> getMidiOutput,
            sequencer::Sequencer *, const std::shared_ptr<sequencer::Clock> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            std::function<bool()> isBouncing,
            const std::function<int()> &getSampleRate,
            const std::function<bool()> &isRecMainWithoutPlaying,
            const std::function<void(int velo, int frameOffset)> &playMetronome,
            std::function<std::shared_ptr<lcdgui::Screens>()>,
            const std::function<bool()> &isNoteRepeatLockedOrPressed,
            const std::shared_ptr<NoteRepeatProcessor> &,
            const std::function<bool()> &isAudioServerCurrentlyRunningOffline);

        void work(int nFrames) override;

        unsigned short getEventFrameOffset() const;

    private:
        EngineHost *engineHost;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        sequencer::Sequencer *sequencer;
        std::shared_ptr<sequencer::Clock> clock;
        std::function<bool()> isBouncing;
        std::function<int()> getSampleRate;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> playMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;
        std::shared_ptr<NoteRepeatProcessor> noteRepeatProcessor;
        std::function<bool()> isAudioServerCurrentlyRunningOffline;

        std::shared_ptr<sequencer::MidiClockOutput> midiClockOutput;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        void triggerClickIfNeeded() const;

        void displayPunchRects() const;

        void stopCountingInIfRequired(SequenceIndex) const;

        std::shared_ptr<sequencer::Sequence> switchToNextSequence() const;

        bool processSongMode() const;

        bool processSeqLoopEnabled() const;

        bool processSeqLoopDisabled() const;

        void processNoteRepeat() const;

        void setTickPositionEffectiveImmediately(int newTickPos, SequenceIndex) const;

        void stopSequencer() const;
    };
} // namespace mpc::engine
