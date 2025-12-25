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

        SamplePreciseTask(const SamplePreciseTask &) = delete;
        SamplePreciseTask &operator=(const SamplePreciseTask &) = delete;

        SamplePreciseTask(SamplePreciseTask &&) noexcept = default;
        SamplePreciseTask &operator=(SamplePreciseTask &&) noexcept = default;
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
            alignas(
                SamplePreciseTask) unsigned char buf[sizeof(SamplePreciseTask)];

            auto *task = reinterpret_cast<SamplePreciseTask *>(buf);
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
            {
                queue.enqueue(std::move(temp[i]));
            }
        }

        void processTasks(const int nFrames)
        {
            SamplePreciseTask temp[Capacity];
            size_t reinsertIndex = 0;
            SamplePreciseTask task;

            while (queue.dequeue(task))
            {
                auto remaining = task.nFrames - task.frameCounter;

                if (remaining < nFrames) // fires in this buffer
                {
                    auto p = std::max<int64_t>(
                        0, std::min<int64_t>(remaining, nFrames - 1));
                    task.f((int)p);
                    task.~SamplePreciseTask();
                }
                else
                {
                    task.frameCounter += nFrames;
                    temp[reinsertIndex++] = std::move(task);
                    task.~SamplePreciseTask();
                }
            }

            for (size_t i = 0; i < reinsertIndex; ++i)
            {
                queue.enqueue(std::move(temp[i]));
            }
        }

    private:
        static constexpr int Capacity = 128;
        BoundedMpmcQueue<SamplePreciseTask, Capacity> queue;
    };
} // namespace mpc::concurrency
