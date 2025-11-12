#include "LayeredScreenTasks.hpp"

using namespace mpc::lcdgui;

void LayeredScreenTasks::post(const std::function<void()> &fn)
{
    std::lock_guard lock(queueMutex);
    queue.push(fn);
}

void LayeredScreenTasks::drain()
{
    std::queue<std::function<void()>> local;

    {
        std::lock_guard lock(queueMutex);
        std::swap(local, queue);
    }

    while (!local.empty())
    {
        local.front()();
        local.pop();
    }
}
