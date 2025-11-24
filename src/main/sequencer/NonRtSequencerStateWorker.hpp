#pragma once

#include "EventState.hpp"
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

        void work() const;

    private:
        std::atomic<bool> running;
        std::thread workerThread;

        Sequencer *sequencer;

        PlaybackState renderPlaybackState(SampleRate, TimeInSamples) const;
    };
} // namespace mpc::sequencer