#pragma once

#include "utils/SmallFn.hpp"

#include <concurrentqueue.h>

namespace mpc::concurrency
{
    class TaskQueue
    {
    public:
        void post(const utils::Task &task)
        {
            queue.enqueue(task);
        }

        void drain()
        {
            utils::Task task;
            while (queue.try_dequeue(task))
            {
                task();
            }
        }

    private:
        moodycamel::ConcurrentQueue<utils::Task> queue;
    };
} // namespace mpc::concurrency