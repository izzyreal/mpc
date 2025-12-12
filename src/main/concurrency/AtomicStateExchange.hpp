#pragma once

#include "concurrency/BoundedMpmcQueue.hpp"

#include "utils/SimpleAction.hpp"

#include <atomic>
#include <functional>
#include <cstdio>
#include <cstdint>
#include <array>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message,
              size_t PoolSize = 3, size_t MessageQueueCapacity = 512>
    class AtomicStateExchange
    {
        using MessageQueue = BoundedMpmcQueue<Message, MessageQueueCapacity>;
        using CallbackQueue =
            BoundedMpmcQueue<utils::SimpleAction, MessageQueueCapacity>;

    protected:
        explicit AtomicStateExchange(std::function<void(State &)> reserveFn)
        {
            actions.reserve(10);
            reserveFn(activeState);

            for (auto &s : pool)
            {
                reserveFn(s);
            }

            currentSnapshot.store(&pool[0], std::memory_order_relaxed);
            writeBuffer = &pool[1];
        }

    public:
        virtual ~AtomicStateExchange() {}

        template <class M> void enqueue(M &&msg) noexcept
        {
            eventMessageQueue.enqueue(Message{std::forward<M>(msg)});
        }

        void enqueueCallback(const utils::SimpleAction &cb) noexcept
        {
            callbackQueue.enqueue(cb);
        }

        void drainQueue() noexcept
        {
            Message msg;

            bool shouldPublish = false;

            while (eventMessageQueue.dequeue(msg))
            {
                applyMessage(msg);
                shouldPublish = true;
            }

            if (shouldPublish)
            {
                publishState();
            }

            for (auto &a : actions)
            {
                a();
            }

            actions.clear();

            utils::SimpleAction cb;

            while (callbackQueue.dequeue(cb))
            {
                cb();
            }
        }

        View getSnapshot() const noexcept
        {
            State *s = currentSnapshot.load(std::memory_order_acquire);
            return View{s};
        }

        void applyMessageImmediate(Message &&msg) noexcept
        {
            applyMessage(msg);
            publishState();

            for (auto &a : actions)
            {
                a();
            }

            actions.clear();

            utils::SimpleAction cb;

            while (callbackQueue.dequeue(cb))
            {
                cb();
            }
        }

    protected:
        virtual void applyMessage(const Message &msg) noexcept = 0;

        State activeState;
        std::array<State, PoolSize> pool;
        std::atomic<State *> currentSnapshot{nullptr};
        State *writeBuffer;
        size_t writeIndex = 1;

        std::vector<utils::SimpleAction> actions;

        void publishState() noexcept
        {
            size_t nextIndex = (writeIndex + 1) % PoolSize;
            State *dst = &pool[nextIndex];

            State *oldSnap = currentSnapshot.load(std::memory_order_acquire);

            if (oldSnap == dst)
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
            currentSnapshot.store(dst, std::memory_order_release);

            writeIndex = nextIndex;
            writeBuffer = dst;
        }

    private:
        std::atomic<uint64_t> publishCount{0};

        MessageQueue eventMessageQueue;
        CallbackQueue callbackQueue;
    };

} // namespace mpc::concurrency
