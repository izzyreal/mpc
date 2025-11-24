#pragma once

#include "EventState.hpp"

#include <thread>
#include <atomic>

namespace mpc::sequencer
{
    class Sequencer;

    struct RenderedEventState
    {
        EventState eventState;
        TimeInSamples timeInSamples;
    };

    struct PlaybackState
    {
        TimeInSamples timeInSamples;
        std::vector<RenderedEventState> events;
    };

    class TrackEventStateWorker
    {
    public:
        explicit TrackEventStateWorker(Sequencer *);
        ~TrackEventStateWorker();

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