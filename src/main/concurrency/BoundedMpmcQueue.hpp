#pragma once

#include <atomic>
#include <cstddef>
#include <cassert>
#include <utility>

namespace mpc::concurrency
{
    template <typename T, size_t Capacity> class BoundedMpmcQueue
    {
    public:
        explicit BoundedMpmcQueue()
            : pad0_{}, buffer_(new cell_t[Capacity]),
              buffer_mask_(Capacity - 1), pad1_{}, pad2_{}, pad3_{}
        {
            assert(Capacity >= 2 && (Capacity & Capacity - 1) == 0);

            for (size_t i = 0; i != Capacity; i++)
            {
                buffer_[i].sequence_.store(i, std::memory_order_relaxed);
            }

            enqueue_pos_.store(0, std::memory_order_relaxed);
            dequeue_pos_.store(0, std::memory_order_relaxed);
        }

        ~BoundedMpmcQueue()
        {
            delete[] buffer_;
        }

        bool enqueue(const T& data) {
            return enqueue_impl(data);
        }

        bool enqueue(T&& data) {
            return enqueue_impl(std::move(data));
        }

        template <class U>
        bool enqueue_impl(U&& data) {
            cell_t* cell;
            size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

            for (;;) {
                cell = &buffer_[pos & buffer_mask_];

                const size_t seq =
                    cell->sequence_.load(std::memory_order_acquire);

                const intptr_t dif =
                    static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

                if (dif == 0) {
                    if (enqueue_pos_.compare_exchange_weak(
                            pos, pos + 1, std::memory_order_relaxed)) {
                        break;
                            }
                } else if (dif < 0) {
                    return false;
                } else {
                    pos = enqueue_pos_.load(std::memory_order_relaxed);
                }
            }

            cell->data_ = std::forward<U>(data);
            cell->sequence_.store(pos + 1, std::memory_order_release);
            return true;
        }

        bool dequeue(T &data)
        {
            cell_t *cell;
            size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

            for (;;)
            {
                cell = &buffer_[pos & buffer_mask_];
                const size_t seq =
                    cell->sequence_.load(std::memory_order_acquire);
                const intptr_t dif =
                    static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
                if (dif == 0)
                {
                    if (dequeue_pos_.compare_exchange_weak(
                            pos, pos + 1, std::memory_order_relaxed))
                    {
                        break;
                    }
                }
                else if (dif < 0)
                {
                    return false;
                }
                else
                {
                    pos = dequeue_pos_.load(std::memory_order_relaxed);
                }
            }
            data = cell->data_;
            cell->sequence_.store(pos + buffer_mask_ + 1,
                                  std::memory_order_release);
            return true;
        }

    private:
        struct cell_t
        {
            std::atomic<size_t> sequence_;
            T data_;
        };

        static constexpr size_t cacheline_size = 64;
        typedef char cacheline_pad_t[cacheline_size];

        cacheline_pad_t pad0_;
        cell_t *const buffer_;
        size_t const buffer_mask_;
        cacheline_pad_t pad1_;
        std::atomic<size_t> enqueue_pos_;
        cacheline_pad_t pad2_;
        std::atomic<size_t> dequeue_pos_;
        cacheline_pad_t pad3_;

        BoundedMpmcQueue(BoundedMpmcQueue const &) = delete;
        void operator=(BoundedMpmcQueue const &) = delete;
    };
} // namespace mpc::concurrency