#include <catch2/catch_test_macros.hpp>

#include "concurrency/SamplePreciseTaskQueue.hpp"

#include <array>
#include <utility>

namespace
{
    struct DtorProbeCallable
    {
        int *destroyCounts = nullptr;
        int *callCount = nullptr;
        int id = -1;

        DtorProbeCallable() = default;

        DtorProbeCallable(int *destroyCountsIn, int *callCountIn, int idIn)
            : destroyCounts(destroyCountsIn), callCount(callCountIn), id(idIn)
        {
        }

        DtorProbeCallable(const DtorProbeCallable &) = delete;
        DtorProbeCallable &operator=(const DtorProbeCallable &) = delete;

        DtorProbeCallable(DtorProbeCallable &&other) noexcept
            : destroyCounts(other.destroyCounts), callCount(other.callCount),
              id(std::exchange(other.id, -1))
        {
        }

        DtorProbeCallable &operator=(DtorProbeCallable &&other) noexcept
        {
            if (this != &other)
            {
                destroyCounts = other.destroyCounts;
                callCount = other.callCount;
                id = std::exchange(other.id, -1);
            }
            return *this;
        }

        ~DtorProbeCallable()
        {
            if (id >= 0)
            {
                destroyCounts[id]++;
            }
        }

        void operator()(int) const
        {
            (*callCount)++;
        }
    };
} // namespace

TEST_CASE("SamplePreciseTaskQueue processTasks does not double-destroy tasks",
          "[sample-precise-task-queue]")
{
    mpc::concurrency::SamplePreciseTaskQueue queue;
    std::array<int, 2> destroyCounts{0, 0};
    int callCount = 0;

    SECTION("task fires in current buffer")
    {
        mpc::concurrency::SamplePreciseTask task;
        task.nFrames = 0;
        task.frameCounter = 0;
        task.f.set(DtorProbeCallable(destroyCounts.data(), &callCount, 0));

        queue.post(std::move(task));
        queue.processTasks(512);

        REQUIRE(callCount == 1);
        REQUIRE(destroyCounts[0] == 1);
    }

    SECTION("task is reinserted before firing")
    {
        mpc::concurrency::SamplePreciseTask task;
        task.nFrames = 1024;
        task.frameCounter = 0;
        task.f.set(DtorProbeCallable(destroyCounts.data(), &callCount, 1));

        queue.post(std::move(task));

        queue.processTasks(512);
        REQUIRE(callCount == 0);
        REQUIRE(destroyCounts[1] == 0);

        queue.processTasks(512);
        REQUIRE(callCount == 0);
        REQUIRE(destroyCounts[1] == 0);

        queue.processTasks(1);
        REQUIRE(callCount == 1);
        REQUIRE(destroyCounts[1] == 1);
    }
}
