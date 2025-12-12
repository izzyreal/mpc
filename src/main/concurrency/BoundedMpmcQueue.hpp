#pragma once

#include <atomic>
#include <cstddef>
#include <cassert>
#include <utility>
#include <type_traits>

namespace mpc::concurrency
{
    template <typename T, size_t Capacity>
    class BoundedMpmcQueue
    {
    public:
        BoundedMpmcQueue()
            : pad0_{}, buffer_(new cell_t[Capacity]),
              buffer_mask_(Capacity - 1), pad1_{}, pad2_{}, pad3_{}
        {
            assert(Capacity >= 2 && ((Capacity & (Capacity - 1)) == 0));

            for (size_t i = 0; i < Capacity; i++)
                buffer_[i].sequence_.store(i, std::memory_order_relaxed);

            enqueue_pos_.store(0, std::memory_order_relaxed);
            dequeue_pos_.store(0, std::memory_order_relaxed);
        }

        ~BoundedMpmcQueue()
        {
            delete[] buffer_;
        }

        bool enqueue(const T& v) { return enqueue_impl(v); }
        bool enqueue(T&& v)      { return enqueue_impl(std::move(v)); }

        template <class U>
        bool enqueue_impl(U&& v)
        {
            cell_t* cell;
            size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

            for (;;) {
                cell = &buffer_[pos & buffer_mask_];

                size_t seq = cell->sequence_.load(std::memory_order_acquire);
                intptr_t dif = (intptr_t)seq - (intptr_t)pos;

                if (dif == 0) {
                    if (enqueue_pos_.compare_exchange_weak(
                            pos, pos + 1, std::memory_order_relaxed))
                        break;
                }
                else if (dif < 0) {
                    return false;
                }
                else {
                    pos = enqueue_pos_.load(std::memory_order_relaxed);
                }
            }

            new (cell->ptr()) T(std::forward<U>(v));   // construct only
            cell->sequence_.store(pos + 1, std::memory_order_release);
            return true;
        }

        bool dequeue(T& out)
        {
            cell_t* cell;
            size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

            for (;;) {
                cell = &buffer_[pos & buffer_mask_];

                size_t seq = cell->sequence_.load(std::memory_order_acquire);
                intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

                if (dif == 0) {
                    if (dequeue_pos_.compare_exchange_weak(
                            pos, pos + 1, std::memory_order_relaxed))
                        break;
                }
                else if (dif < 0) {
                    return false;
                }
                else {
                    pos = dequeue_pos_.load(std::memory_order_relaxed);
                }
            }

            T* p = cell->ptr();
            out = std::move(*p);    // move out
            p->~T();                // destroy ONLY on consumer thread

            cell->sequence_.store(pos + buffer_mask_ + 1,
                                  std::memory_order_release);
            return true;
        }

    private:
        struct cell_t
        {
            std::atomic<size_t> sequence_;
            typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;

            T* ptr()       { return reinterpret_cast<T*>(&storage); }
            T* ptr() const { return reinterpret_cast<T*>(const_cast<void*>(
                                  static_cast<const void*>(&storage))); }
        };

        static constexpr size_t cacheline_size = 64;
        using pad = char[cacheline_size];

        pad pad0_;
        cell_t* const buffer_;
        const size_t buffer_mask_;
        pad pad1_;
        std::atomic<size_t> enqueue_pos_;
        pad pad2_;
        std::atomic<size_t> dequeue_pos_;
        pad pad3_;

        BoundedMpmcQueue(const BoundedMpmcQueue&) = delete;
        void operator=(const BoundedMpmcQueue&)   = delete;
    };
}
