#pragma once
#include <cstddef>
#include <type_traits>
#include <concurrentqueue.h>

namespace mpc::concurrency {

template <typename T, std::size_t Capacity>
class FreeList {
public:
    FreeList() {
        static_assert(std::is_trivially_destructible_v<T>,
                      "FreeList requires trivially destructible T");

        // Pre-allocate raw memory for all objects
        for (std::size_t i = 0; i < Capacity; ++i) {
            T* ptr = reinterpret_cast<T*>(&storage[i]);
            queue_.enqueue(ptr);
        }
    }

    // Get one object (uninitialized)
    bool acquire(T*& out) {
        return queue_.try_dequeue(out);
    }

    // Return the object (user must ensure T is "destroyed" if needed)
    void release(T* t) {
        queue_.enqueue(t);
    }

    static constexpr std::size_t max_size() { return Capacity; }

private:
    using Storage = std::aligned_storage_t<sizeof(T), alignof(T)>;
    Storage storage[Capacity];

    moodycamel::ConcurrentQueue<T*> queue_;
};

} // namespace mpc::concurrency
