#pragma once

#include <atomic>
#include <memory>
#include <functional>
#include <cstdio>
#include <array>
#include <concurrentqueue.h>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message, size_t PoolSize = 3>
    class AtomicStateExchange
    {
        using MessageQueue = moodycamel::ConcurrentQueue<
            Message, moodycamel::ConcurrentQueueDefaultTraits>;

    protected:
        explicit AtomicStateExchange(std::function<void(State&)> reserveFn,
                                     size_t messageQueueCapacity = 512)
        {
            reserveFn(activeState);

            for (auto& s : pool)
            {
                reserveFn(s);
            }

            currentSnapshot = std::shared_ptr<State>(&pool[0], [](State*) {});
            writeBuffer = &pool[1];

            eventMessageQueue =
                std::make_shared<MessageQueue>(messageQueueCapacity);
        }

    public:
        void enqueue(Message&& msg) const noexcept
        {
            eventMessageQueue->enqueue(std::move(msg));
        }

        void drainQueue() noexcept
        {
            Message msg;

            bool shouldPublish = false;

            while (eventMessageQueue->try_dequeue(msg))
            {
                applyMessage(msg);
                shouldPublish = true;
            }

            if (shouldPublish)
            {
                publishState();
            }
        }

        View getSnapshot() const noexcept
        {
            auto s = std::atomic_load_explicit(&currentSnapshot,
                                               std::memory_order_acquire);
            return View{std::move(s)};
        }

    protected:
        void publishState() noexcept
        {
            size_t nextIndex = (writeIndex + 1) % PoolSize;
            State* dst = &pool[nextIndex];

            auto oldSnap = std::atomic_load_explicit(&currentSnapshot,
                                                     std::memory_order_acquire);

            if (oldSnap.get() == dst)
            {
                uint64_t n = publishCount.fetch_add(1, std::memory_order_relaxed) + 1;

                std::fprintf(stdout,
                             "[AtomicStateExchange] WARNING: Overwriting buffer still held by readers at publish #%llu\n",
                             static_cast<unsigned long long>(n));

                std::fflush(stdout);
            }

            *dst = activeState;
            auto newShared = std::shared_ptr<State>(dst, [](State*) {});
            std::atomic_store(&currentSnapshot, newShared);

            writeIndex = nextIndex;
            writeBuffer = dst;
        }

        virtual void applyMessage(const Message& msg) noexcept = 0;

        State activeState;
        std::array<State, PoolSize> pool;
        std::shared_ptr<State> currentSnapshot;
        State* writeBuffer;
        size_t writeIndex = 1;

    private:
        std::atomic<uint64_t> publishCount{0};
        std::shared_ptr<MessageQueue> eventMessageQueue;
    };

} // namespace mpc::concurrency

