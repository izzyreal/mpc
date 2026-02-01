#pragma once

#include <memory>
#include <thread>
#include <atomic>

namespace mpc
{
    class Mpc;
    class SaveTarget;

    class AutoSave
    {
    public:
        AutoSave() = default;
        ~AutoSave();

        void restoreAutoSavedState(Mpc &, std::shared_ptr<SaveTarget>,
                                   bool headless);

        static void storeAutoSavedState(Mpc &,
                                        const std::shared_ptr<SaveTarget> &);

        void interruptRestorationIfStillOngoing();

    private:
        std::thread restoreThread;
        std::atomic<bool> shouldStopRestore{false};
    };
} // namespace mpc
