#pragma once

#include <functional>
#include <mutex>
#include <queue>

namespace mpc::lcdgui
{
    class LayeredScreenTasks
    {
    public:
        void post(const std::function<void()> &fn);
        void drain();

    private:
        std::mutex queueMutex;
        std::queue<std::function<void()>> queue;
    };
} // namespace mpc::lcdgui
