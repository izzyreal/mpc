#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <functional>

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
                                   bool headless,
                                   std::function<void()> onComplete = {});

        static void storeAutoSavedState(Mpc &,
                                        const std::shared_ptr<SaveTarget> &);

        void interruptRestorationIfStillOngoing();

    private:
        std::thread restoreThread;
        std::atomic<bool> shouldStopRestore{false};
    };
} // namespace mpc
