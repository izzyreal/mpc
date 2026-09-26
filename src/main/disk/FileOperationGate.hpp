#pragma once

#include <atomic>
#include <memory>

namespace mpc::disk
{
    // Admission only; never held by the audio thread. A lease follows a legacy
    // worker or the complete managed operation, including presentation delays.
    class FileOperationGate
    {
        std::shared_ptr<std::atomic<int>> active =
            std::make_shared<std::atomic<int>>(0);

    public:
        bool isManagedSaveActive() const
        {
            return active->load() == 2;
        }
        std::shared_ptr<void> tryAcquire(bool save = false)
        {
            int expected = 0;
            if (!active->compare_exchange_strong(expected, save ? 2 : 1))
            {
                return {};
            }
            return std::shared_ptr<void>(active.get(),
                                         [state = active](void *)
                                         {
                                             state->store(0);
                                         });
        }
    };
} // namespace mpc::disk
