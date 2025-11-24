#pragma once

#include "PlaybackState.hpp"

#include <thread>
#include <atomic>

namespace mpc::sequencer
{
    class Sequencer;

    class NonRtSequencerStateWorker
    {
    public:
        explicit NonRtSequencerStateWorker(Sequencer *);
        ~NonRtSequencerStateWorker();

        void start();
        void stop();

        void work();

    private:
        std::atomic<bool> running;
        std::thread workerThread;

        Sequencer *sequencer;

        TimeInSamples lastRenderedTimeInSamples{-1};

        PlaybackState renderPlaybackState(SampleRate, TimeInSamples) const;
    };
} // namespace mpc::sequencer