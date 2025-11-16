#pragma once

#include <atomic>
#include <memory>
#include <functional>
#include <cstdio>
#include <cstdint>
#include <array>
#include <concurrentqueue.h>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message,
              size_t PoolSize = 3>
    class AtomicStateExchange
    {
        struct SequencedMessage
        {
            uint64_t seq;
            Message payload;
        };

        using MessageQueue = moodycamel::ConcurrentQueue<SequencedMessage>;
        mutable std::atomic<uint64_t> globalSeq{0};
        std::vector<SequencedMessage> sequencedMessages;

    protected:
        explicit AtomicStateExchange(std::function<void(State &)> reserveFn,
                                     size_t messageQueueCapacity = 512)
        {
            actions.reserve(10);
            reserveFn(activeState);

            for (auto &s : pool)
            {
                reserveFn(s);
            }

            currentSnapshot = std::shared_ptr<State>(&pool[0], [](State *) {});
            writeBuffer = &pool[1];

            eventMessageQueue =
                std::make_shared<MessageQueue>(messageQueueCapacity);
            sequencedMessages.reserve(messageQueueCapacity);
        }

    public:
        virtual ~AtomicStateExchange() = default;

        virtual void enqueue(Message &msg) const noexcept
        {
            SequencedMessage sm;
            sm.seq = globalSeq.fetch_add(1, std::memory_order_relaxed);
            sm.payload = std::move(msg);
            eventMessageQueue->enqueue(std::move(sm));
        }

        virtual void enqueue(Message &&msg) const noexcept
        {
            SequencedMessage sm;
            sm.seq = globalSeq.fetch_add(1, std::memory_order_relaxed);
            sm.payload = std::move(msg);
            eventMessageQueue->enqueue(std::move(sm));
        }

        void drainQueue() noexcept
        {
            SequencedMessage sm;

            bool shouldPublish = false;

            while (eventMessageQueue->try_dequeue(sm))
            {
                sequencedMessages.push_back(std::move(sm));
                shouldPublish = true;
            }

            std::sort(sequencedMessages.begin(), sequencedMessages.end(),
                      [](const auto &a, const auto &b)
                      {
                          return a.seq < b.seq;
                      });

            for (auto it = sequencedMessages.begin();
                 it != sequencedMessages.end();)
            {
                auto msg = std::move(it->payload);
                it = sequencedMessages.erase(it);
                applyMessage(msg);
            }

            sequencedMessages.clear();

            if (shouldPublish)
            {
                publishState();
            }

            for (auto &a : actions)
            {
                a();
            }

            actions.clear();
        }

        View getSnapshot() const noexcept
        {
            auto s = std::atomic_load_explicit(&currentSnapshot,
                                               std::memory_order_acquire);
            return View{std::move(s)};
        }

    protected:
        virtual void applyMessage(const Message &msg) noexcept = 0;

        State activeState;
        std::array<State, PoolSize> pool;
        std::shared_ptr<State> currentSnapshot;
        State *writeBuffer;
        size_t writeIndex = 1;
        std::vector<std::function<void()>> actions;

        void publishState() noexcept
        {
            size_t nextIndex = (writeIndex + 1) % PoolSize;
            State *dst = &pool[nextIndex];

            auto oldSnap = std::atomic_load_explicit(&currentSnapshot,
                                                     std::memory_order_acquire);

            if (oldSnap.get() == dst)
            {
                const uint64_t n =
                    publishCount.fetch_add(1, std::memory_order_relaxed) + 1;

                std::fprintf(stdout,
                             "[AtomicStateExchange] WARNING: Overwriting "
                             "buffer still held by readers at publish #%llu\n",
                             static_cast<unsigned long long>(n));

                std::fflush(stdout);
            }

            *dst = activeState;
            auto newShared = std::shared_ptr<State>(dst, [](State *) {});
            std::atomic_store(&currentSnapshot, newShared);

            writeIndex = nextIndex;
            writeBuffer = dst;
        }

    private:
        std::atomic<uint64_t> publishCount{0};
        std::shared_ptr<MessageQueue> eventMessageQueue;
    };

} // namespace mpc::concurrency
