#pragma once

#include "utils/SmallFn.hpp"

#include <concurrentqueue.h>

#include <cstdint>

namespace mpc::concurrency
{
    struct SamplePreciseTask
    {
        utils::SmallFn<96, void(int)> f;
        int64_t nFrames = 0;
        int64_t frameCounter = 0;
        bool midiNoteOff = false;
    };

    class SamplePreciseTaskQueue
    {
    public:
        SamplePreciseTaskQueue()
        {
            queue = moodycamel::ConcurrentQueue<SamplePreciseTask>(Capacity);
        }

        void post(const SamplePreciseTask &task)
        {
            queue.enqueue(task);
        }

        void flushMidiNoteOffs()
        {
            SamplePreciseTask batch[Capacity];
            SamplePreciseTask temp[Capacity];

            const size_t queueSize = queue.try_dequeue_bulk(batch, Capacity);
            size_t reinsertSize = 0;

            for (size_t i = 0; i < queueSize; ++i)
            {
                auto &task = batch[i];

                if (task.midiNoteOff)
                {
                    task.f(0);
                    continue;
                }

                temp[reinsertSize++] = task;
            }

            for (size_t i = 0; i < reinsertSize; ++i)
            {
                queue.enqueue(std::move(temp[i]));
            }
        }

        void processTasks(const int nFrames)
        {
            SamplePreciseTask batch[Capacity];
            SamplePreciseTask temp[Capacity];

            const size_t queueSize = queue.try_dequeue_bulk(batch, Capacity);
            size_t reinsertSize = 0;

            for (size_t i = 0; i < queueSize; ++i)
            {
                auto &task = batch[i];

                if (const auto candidatePosInBuffer =
                        task.frameCounter + nFrames - task.nFrames;
                    candidatePosInBuffer >= 0)
                {
                    const auto posInBuffer =
                        std::min(candidatePosInBuffer,
                                 static_cast<int64_t>(nFrames - 1));

                    task.f(posInBuffer);
                    continue;
                }

                task.frameCounter += nFrames;

                temp[reinsertSize++] = task;
            }

            for (size_t i = 0; i < reinsertSize; ++i)
            {
                queue.enqueue(std::move(temp[i]));
            }
        }

    private:
        static constexpr int Capacity = 100;
        moodycamel::ConcurrentQueue<SamplePreciseTask> queue;
    };
} // namespace mpc::concurrency