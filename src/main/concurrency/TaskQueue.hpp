#pragma once

#include "utils/SmallFn.hpp"

#include <concurrentqueue.h>

namespace mpc::concurrency
{
    using Task = utils::SmallFn<96, void()>;
    class TaskQueue
    {
    public:
        void post(const Task &task)
        {
            queue.enqueue(task);
        }

        void drain()
        {
            Task task;
            while (queue.try_dequeue(task))
            {
                task();
            }
        }

    private:
        moodycamel::ConcurrentQueue<Task> queue;
    };
} // namespace mpc::concurrency