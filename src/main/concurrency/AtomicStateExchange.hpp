#pragma once

#include "concurrency/BoundedMpmcQueue.hpp"
#include "utils/SimpleAction.hpp"

#include <atomic>
#include <functional>
#include <cstdio>
#include <cstdint>
#include <array>
#include <new>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message,
              size_t PoolSize = 3, size_t MessageQueueCapacity = 512>
    class AtomicStateExchange
    {
        using MessageQueue = BoundedMpmcQueue<Message, MessageQueueCapacity>;
        using CallbackQueue =
            BoundedMpmcQueue<utils::SimpleAction, MessageQueueCapacity>;

    public:
        class OwnerScopeGuard
        {
        public:
            OwnerScopeGuard(AtomicStateExchange *exchange,
                            const bool publishOnExit) noexcept
                : exchange(exchange), publishOnExit(publishOnExit)
            {
                exchange->beginOwnerScope();
            }

            OwnerScopeGuard(const OwnerScopeGuard &) = delete;
            OwnerScopeGuard &operator=(const OwnerScopeGuard &) = delete;

            OwnerScopeGuard(OwnerScopeGuard &&other) noexcept
                : exchange(other.exchange), publishOnExit(other.publishOnExit)
            {
                other.exchange = nullptr;
            }

            OwnerScopeGuard &operator=(OwnerScopeGuard &&other) noexcept
            {
                if (this == &other)
                {
                    return *this;
                }

                if (exchange)
                {
                    exchange->endOwnerScope(publishOnExit);
                }

                exchange = other.exchange;
                publishOnExit = other.publishOnExit;
                other.exchange = nullptr;

                return *this;
            }

            ~OwnerScopeGuard()
            {
                if (exchange)
                {
                    exchange->endOwnerScope(publishOnExit);
                }
            }

        private:
            AtomicStateExchange *exchange{nullptr};
            bool publishOnExit{true};
        };

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
            queue.enqueue(Message(std::forward<M>(msg)));
        }

        void enqueueCallback(utils::SimpleAction cb) noexcept
        {
            callbackQueue.enqueue(std::move(cb));
        }

        void drainQueue() noexcept
        {
            alignas(Message) unsigned char msgBuf[sizeof(Message)];
            auto *msg = reinterpret_cast<Message *>(msgBuf);

            bool shouldPublish = false;

            while (queue.dequeue(*msg))
            {
                applyMessage(*msg);
                msg->~Message();
                shouldPublish = true;
            }

            if (shouldPublish)
            {
                if (isOwnerScopeActiveOnCurrentThread())
                {
                    pendingPublish = true;
                }
                else
                {
                    publishState();
                    pendingPublish = false;
                }
            }

            for (auto &a : actions)
            {
                a();
            }

            actions.clear();

            alignas(utils::SimpleAction) unsigned char
                cbBuf[sizeof(utils::SimpleAction)];
            auto *cb = reinterpret_cast<utils::SimpleAction *>(cbBuf);

            while (callbackQueue.dequeue(*cb))
            {
                (*cb)();
                cb->~SmallFn();
            }
        }

        View getSnapshot() const noexcept
        {
            if (isOwnerScopeActiveOnCurrentThread())
            {
                return View{&activeState};
            }

            State *s = currentSnapshot.load(std::memory_order_acquire);
            return View{s};
        }

        void applyMessageImmediate(Message &&msg) noexcept
        {
            applyMessage(msg);

            if (isOwnerScopeActiveOnCurrentThread())
            {
                pendingPublish = true;
            }
            else
            {
                publishState();
                pendingPublish = false;
            }

            for (auto &a : actions)
            {
                a();
            }

            actions.clear();

            alignas(utils::SimpleAction) unsigned char
                cbBuf[sizeof(utils::SimpleAction)];
            auto *cb = reinterpret_cast<utils::SimpleAction *>(cbBuf);

            while (callbackQueue.dequeue(*cb))
            {
                (*cb)();
                cb->~SmallFn();
            }
        }

        OwnerScopeGuard scopedOwnerAccess(
            const bool publishOnExit = true) noexcept
        {
            return OwnerScopeGuard(this, publishOnExit);
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
            writeBuffer = dst;
        }

    private:
        void beginOwnerScope() noexcept
        {
            if (tlsOwnerScopeInstance == nullptr)
            {
                tlsOwnerScopeInstance = this;
                tlsOwnerScopeDepth = 1;
                return;
            }

            if (tlsOwnerScopeInstance == this)
            {
                ++tlsOwnerScopeDepth;
            }
        }

        void endOwnerScope(const bool publishOnExit) noexcept
        {
            if (tlsOwnerScopeInstance != this || tlsOwnerScopeDepth == 0)
            {
                return;
            }

            --tlsOwnerScopeDepth;

            if (tlsOwnerScopeDepth > 0)
            {
                return;
            }

            if (publishOnExit && pendingPublish)
            {
                publishState();
                pendingPublish = false;
            }

            tlsOwnerScopeInstance = nullptr;
        }

        bool isOwnerScopeActiveOnCurrentThread() const noexcept
        {
            return tlsOwnerScopeInstance == this && tlsOwnerScopeDepth > 0;
        }

        std::atomic<uint64_t> publishCount{0};
        bool pendingPublish{false};

        inline static thread_local AtomicStateExchange *tlsOwnerScopeInstance{
            nullptr};
        inline static thread_local uint32_t tlsOwnerScopeDepth{0};

        MessageQueue queue;
        CallbackQueue callbackQueue;
    };

} // namespace mpc::concurrency
