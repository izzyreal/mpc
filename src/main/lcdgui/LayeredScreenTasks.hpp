#pragma once

#include <functional>
#include <mutex>
#include <queue>

namespace mpc::lcdgui
{
    class LayeredScreenTasks
    {
    public:
        // Uses a lock, so don't invoke on the audio thread.
        // It is meant to be used from worker threads.
        void post(const std::function<void()> &fn);
        void drain();

    private:
        std::mutex queueMutex;
        std::queue<std::function<void()>> queue;
    };
} // namespace mpc::lcdgui
