#pragma once

#include <atomic>
#include <memory>
#include <functional>

#include <concurrentqueue.h>

namespace mpc::concurrency
{
    template <typename State, typename View, typename Message>
    class AtomicStateExchange
    {
        using MessageQueue = moodycamel::ConcurrentQueue<
            Message, moodycamel::ConcurrentQueueDefaultTraits>;

    protected:
        explicit AtomicStateExchange(std::function<void(State&)> reserveFn, size_t messageQueueCapacity = 512)
        {
            reserveFn(activeState);
            reserveFn(bufferA);
            reserveFn(bufferB);
        
            currentSnapshot = std::shared_ptr<State>(&bufferA, [](State*) {});
            writeBuffer = &bufferB;

            eventMessageQueue =
                std::make_shared<MessageQueue>(messageQueueCapacity);
        }

    public:
        void enqueue(Message &&msg) const noexcept
        {
            eventMessageQueue->enqueue(std::move(msg));
        }

        void drainQueue() noexcept
        {
            Message msg;

            while (eventMessageQueue->try_dequeue(msg))
            {
                applyMessage(msg);
            }

            publishState();
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
            State *dst = writeBuffer;
            *dst = activeState;
            auto newShared = std::shared_ptr<State>(dst, [](State *) {});
            std::atomic_store(&currentSnapshot, newShared);
            writeBuffer = (dst == &bufferA) ? &bufferB : &bufferA;
        }

        virtual void applyMessage(const Message &msg) noexcept = 0;
        State activeState;
        State bufferA;
        State bufferB;
        std::shared_ptr<State> currentSnapshot;
        State *writeBuffer;

    private:
        std::shared_ptr<MessageQueue> eventMessageQueue;
    };

} // namespace mpc::concurrency
