#pragma once

#include "utils/SmallFn.hpp"

#include "concurrency/BoundedMpmcQueue.hpp"

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

            while (queue.dequeue(task))
            {
                task();
            }
        }

    private:
        static constexpr size_t CAPACITY = 64;
        BoundedMpmcQueue<utils::Task, CAPACITY> queue;
    };
} // namespace mpc::concurrency