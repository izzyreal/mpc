#pragma once

#include <atomic>
#include <thread>

namespace mpc::concurrency
{
    struct SpinLock
    {
        std::atomic<bool> flag{false};

        bool try_acquire()
        {
            bool expected = false;
            return flag.compare_exchange_strong(expected, true,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed);
        }

        void release()
        {
            flag.store(false, std::memory_order_release);
        }

        void acquire()
        {
            for (;;)
            {
                if (try_acquire())
                {
                    return;
                }
                std::this_thread::yield();
            }
        }
    };
}
