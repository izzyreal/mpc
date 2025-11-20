#pragma once

#include <thread>
#include <atomic>

namespace mpc::sequencer
{
    class Sequencer;

    class TrackEventStateWorker
    {
    public:
        explicit TrackEventStateWorker(Sequencer *);
        ~TrackEventStateWorker();

        void start();
        void stop();

    private:
        std::atomic<bool> running;
        std::thread workerThread;

        Sequencer *sequencer;

        void work() const;
    };
} // namespace mpc::sequencer