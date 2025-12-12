#pragma once

#include "utils/SmallFn.hpp"
#include "concurrency/BoundedMpmcQueue.hpp"

namespace mpc::concurrency
{
    class TaskQueue
    {
    public:
        void post(utils::Task &task)
        {
            queue.enqueue(std::move(task));
        }

        void drain()
        {
            alignas(utils::Task) unsigned char buf[sizeof(utils::Task)];
            auto* task = reinterpret_cast<utils::Task*>(buf);

            while (queue.dequeue(*task))
            {
                (*task)();
                task->~SmallFn();
            }
        }

    private:
        static constexpr size_t CAPACITY = 64;
        BoundedMpmcQueue<utils::Task, CAPACITY> queue;
    };
} // namespace mpc::concurrency
