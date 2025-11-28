#pragma once

#include "FloatTypes.hpp"
#include "PlaybackState.hpp"
#include "lcdgui/ScreenId.hpp"

#include <functional>
#include <thread>
#include <atomic>

namespace mpc::sequencer
{
    constexpr TimeInSamples playbackStateValiditySafetyMarginTimeInSamples = 10000;

    class Sequencer;

    class SequencerStateWorker
    {
    public:
        explicit SequencerStateWorker(
            const std::function<bool(std::initializer_list<lcdgui::ScreenId>)> &isCurrentScreen,
            const std::function<bool()>& isRecMainWithoutPlaying,
            Sequencer *);
        ~SequencerStateWorker();

        void start();
        void stop();

        void stopAndWaitUntilStopped();

        void work() const;

        void refreshPlaybackState(PositionQuarterNotes playOffset, TimeInSamples,
                             const std::function<void()> &onComplete) const;

        Sequencer *getSequencer() const;

    private:
        std::atomic<bool> running;
        std::thread workerThread;

        const std::function<bool(std::initializer_list<lcdgui::ScreenId>)> isCurrentScreen;
        const std::function<bool()> isRecMainWithoutPlaying;

        Sequencer *sequencer;

        PlaybackState renderPlaybackState(SampleRate, PositionQuarterNotes playOffset, TimeInSamples) const;
    };
} // namespace mpc::sequencer