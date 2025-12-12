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
            alignas(utils::Task) unsigned char buf[sizeof(utils::Task)];

            while (true) {
                if (!queue.dequeue(*reinterpret_cast<utils::Task*>(buf)))
                    break;

                auto* task = reinterpret_cast<utils::Task*>(buf);
                (*task)();
                task->~SmallFn();
            }
        }

    private:
        static constexpr size_t CAPACITY = 64;
        BoundedMpmcQueue<utils::Task, CAPACITY> queue;
    };
} // namespace mpc::concurrency