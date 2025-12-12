#pragma once

#include "concurrency/BoundedMpmcQueue.hpp"
#include "utils/SmallFn.hpp"

#include <cstdint>
#include <new>

namespace mpc::concurrency
{
    struct SamplePreciseTask
    {
        utils::SmallFn<96, void(int)> f;
        int64_t nFrames = 0;
        int64_t frameCounter = 0;
        bool noteOff = false;

        SamplePreciseTask() = default;

        SamplePreciseTask(const SamplePreciseTask&) = delete;
        SamplePreciseTask& operator=(const SamplePreciseTask&) = delete;

        SamplePreciseTask(SamplePreciseTask&&) noexcept = default;
        SamplePreciseTask& operator=(SamplePreciseTask&&) noexcept = default;
    };

    class SamplePreciseTaskQueue
    {
    public:
        void post(SamplePreciseTask task)
        {
            queue.enqueue(std::move(task));
        }

        void flushNoteOffs()
        {
            alignas(SamplePreciseTask)
            unsigned char buf[sizeof(SamplePreciseTask)];

            auto* task = reinterpret_cast<SamplePreciseTask*>(buf);
            SamplePreciseTask temp[Capacity];
            size_t reinsertIndex = 0;

            while (queue.dequeue(*task))
            {
                if (task->noteOff)
                {
                    task->f(0);
                    task->~SamplePreciseTask();
                    continue;
                }

                temp[reinsertIndex++] = std::move(*task);
                task->~SamplePreciseTask();
            }

            for (size_t i = 0; i < reinsertIndex; ++i)
                queue.enqueue(std::move(temp[i]));
        }

        void processTasks(const int nFrames)
        {
            alignas(SamplePreciseTask)
            unsigned char buf[sizeof(SamplePreciseTask)];

            auto* task = reinterpret_cast<SamplePreciseTask*>(buf);
            SamplePreciseTask temp[Capacity];
            size_t reinsertIndex = 0;

            while (queue.dequeue(*task))
            {
                if (auto c = task->frameCounter + nFrames - task->nFrames; c >= 0)
                {
                    const auto p = std::min(c, static_cast<int64_t>(nFrames - 1));
                    task->f(static_cast<int>(p));
                    task->~SamplePreciseTask();
                    continue;
                }

                task->frameCounter += nFrames;
                temp[reinsertIndex++] = std::move(*task);
                task->~SamplePreciseTask();
            }

            for (size_t i = 0; i < reinsertIndex; ++i)
                queue.enqueue(std::move(temp[i]));
        }

    private:
        static constexpr int Capacity = 128;
        BoundedMpmcQueue<SamplePreciseTask, Capacity> queue;
    };
} // namespace mpc::concurrency
