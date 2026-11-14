#pragma once

#include "concurrency/BoundedMpmcQueue.hpp"
#include "utils/SimpleAction.hpp"

#include <atomic>
#include <cassert>
#include <functional>
#include <cstdio>
#include <cstdint>
#include <array>
#include <new>
#include <optional>
#include <memory>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message,
              size_t PoolSize = 3, size_t MessageQueueCapacity = 512,
              size_t DeferredCapacity = 0>
    class AtomicStateExchange
    {
        using MessageQueue = BoundedMpmcQueue<Message, MessageQueueCapacity>;
        using CallbackQueue =
            BoundedMpmcQueue<utils::SimpleAction, MessageQueueCapacity>;

    protected:
        explicit AtomicStateExchange(std::function<void(State &)> reserveFn)
            : deferred(DeferredCapacity
                           ? new std::optional<Message>[DeferredCapacity]
                           : nullptr)
        {
            actions.reserve(10);
            reserveFn(activeState);

            for (auto &s : pool)
            {
                reserveFn(s);
            }

            currentSnapshot.store(&pool[0], std::memory_order_relaxed);
        }

    public:
        virtual ~AtomicStateExchange() {}

        // Owner-thread only. A lock-contended message must not compete with
        // producers for space in the MPMC queue after it has been dequeued.
        // Returns false after reporting an overflow to the derived class.
        template <class M> bool defer(const M &msg) noexcept
        {
            static_assert(DeferredCapacity > 0,
                          "Deferred messages require reserved capacity");
            if (deferredCount == DeferredCapacity)
            {
                deferredOverflowCount.fetch_add(1, std::memory_order_relaxed);
                onDeferredOverflow(Message(msg));
                deferredThisApply = true;
                return false;
            }
            deferred[deferredTail].emplace(Message(msg));
            deferredTail = (deferredTail + 1) % DeferredCapacity;
            ++deferredCount;
            deferredThisApply = true;
            return true;
        }

        uint64_t getDeferredOverflowCount() const noexcept
        {
            return deferredOverflowCount.load(std::memory_order_relaxed);
        }

        template <class M> bool enqueue(M &&msg) noexcept
        {
            return queue.enqueue(Message(std::forward<M>(msg)));
        }

        void enqueueCallback(utils::SimpleAction cb) noexcept
        {
            callbackQueue.enqueue(std::move(cb));
        }

        // Must be called by the owner thread for this state exchange. For
        // PerformanceManager, that is the audio thread; other threads should
        // enqueue messages and, if needed, enqueue a callback to observe
        // completion after the owner thread has published the new state.
        void drainQueue() noexcept
        {
            alignas(Message) unsigned char msgBuf[sizeof(Message)];
            auto *msg = reinterpret_cast<Message *>(msgBuf);

            bool shouldPublish = false;
            bool queueExhausted = false;

            // Allow follow-up messages from handlers, but cap retries of a
            // contended message to finite work in this audio block.
            for (size_t i = 0; i < MessageQueueCapacity; ++i)
            {
                deferredThisApply = false;
                bool haveMessage = false;
                if constexpr (DeferredCapacity > 0)
                {
                    if (deferredCount)
                    {
                        auto &pending = deferred[deferredHead];
                        new (msg) Message(std::move(*pending));
                        pending.reset();
                        deferredHead = (deferredHead + 1) % DeferredCapacity;
                        --deferredCount;
                        haveMessage = true;
                    }
                }
                if (!haveMessage && !queue.dequeueUninitialized(msg))
                {
                    queueExhausted = true;
                    break;
                }
                applyMessage(*msg);
                msg->~Message();
                shouldPublish = true;
                if (deferredThisApply) break;
            }

            if (shouldPublish)
            {
                publishState();
            }

            if (queueExhausted && !deferredCount) finishCallbacks();
        }

        View getSnapshot() const noexcept
        {
            State *s = currentSnapshot.load(std::memory_order_acquire);
            return View{s};
        }

        void applyMessageImmediate(Message &&msg) noexcept
        {
            deferredThisApply = false;
            applyMessage(msg);
            publishState();
            // Queued callbacks belong to queued work; drainQueue completes them.
        }

    protected:
        bool hasDeferredMessages() const noexcept { return deferredCount != 0; }

        template <class F> void discardQueuedMessages(F &&discard)
        {
            if constexpr (DeferredCapacity > 0)
            {
                while (deferredCount)
                {
                    auto &pending = deferred[deferredHead];
                    discard(*pending);
                    pending.reset();
                    deferredHead = (deferredHead + 1) % DeferredCapacity;
                    --deferredCount;
                }
            }
            alignas(Message) unsigned char storage[sizeof(Message)];
            auto *message = reinterpret_cast<Message *>(storage);
            while (queue.dequeueUninitialized(message))
            {
                discard(*message);
                message->~Message();
            }
        }

        virtual void applyMessage(const Message &msg) noexcept = 0;
        // Release resources owned by a message that cannot be retried.
        virtual void onDeferredOverflow(const Message &) noexcept {}

        State activeState;
        std::array<State, PoolSize> pool;
        std::atomic<State *> currentSnapshot{nullptr};
        size_t writeIndex = 1;

        std::vector<utils::SimpleAction> actions;

        void publishState() noexcept
        {
            const size_t nextIndex = (writeIndex + 1) % PoolSize;
            State *dst = &pool[nextIndex];

            State *oldSnap = currentSnapshot.load(std::memory_order_acquire);

            if (oldSnap == dst)
            {
                const uint64_t n =
                    publishCount.fetch_add(1, std::memory_order_relaxed) + 1;

                std::fprintf(stdout,
                             "[AtomicStateExchange] WARNING: Overwriting "
                             "buffer still held by readers at publish #%llu\n",
                             (unsigned long long)n);
                std::fflush(stdout);
            }

            *dst = activeState;
            currentSnapshot.store(dst, std::memory_order_release);

            writeIndex = nextIndex;
        }

    private:
        std::unique_ptr<std::optional<Message>[]> deferred;
        size_t deferredHead = 0, deferredTail = 0, deferredCount = 0;
        bool deferredThisApply = false;

        void finishCallbacks()
        {
            for (auto &a : actions) a();
            actions.clear();
            alignas(utils::SimpleAction) unsigned char
                cbBuf[sizeof(utils::SimpleAction)];
            auto *cb = reinterpret_cast<utils::SimpleAction *>(cbBuf);
            while (callbackQueue.dequeueUninitialized(cb))
            {
                (*cb)();
                cb->~SmallFn();
            }
        }

        std::atomic<uint64_t> publishCount{0};
        std::atomic<uint64_t> deferredOverflowCount{0};

        MessageQueue queue;
        CallbackQueue callbackQueue;
    };

} // namespace mpc::concurrency
