#pragma once

#include "PlaybackState.hpp"
#include "lcdgui/ScreenId.hpp"

#include <functional>
#include <thread>
#include <atomic>

namespace mpc::sequencer
{
    class SequencerStateView;
    class Sequencer;

    class SequencerStateWorker
    {
    public:
        explicit SequencerStateWorker(
            const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
                &isCurrentScreen,
            const std::function<bool()> &isRecMainWithoutPlaying, Sequencer *);
        ~SequencerStateWorker();

        void start();
        void stop();

        void stopAndWaitUntilStopped();

        void work() const;

        Sequencer *getSequencer() const;

    private:
        std::atomic<bool> running;
        std::thread workerThread;

        const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
            isCurrentScreen;
        const std::function<bool()> isRecMainWithoutPlaying;

        Sequencer *sequencer;

        void
        refreshPlaybackState(const PlaybackState &previousPlaybackState,
                             TimeInSamples,
                             const std::function<void()> &onComplete) const;

        PlaybackState renderPlaybackState(SampleRate,
                                          const PlaybackState &prevState,
                                          TimeInSamples,
                                          const SequencerStateView &) const;
    };
} // namespace mpc::sequencer