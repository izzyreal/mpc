#pragma once

#include <concurrentqueue.h>

namespace mpc::concurrency
{
    class TaskQueue {
    public:
        void post(const std::function<void()>& fn) {
            queue.enqueue(fn);
        }

        void drain() {
            std::function<void()> fn;
            while (queue.try_dequeue(fn)) fn();
        }

    private:
        moodycamel::ConcurrentQueue<std::function<void()>> queue;
    };
}