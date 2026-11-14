#pragma once
#include "TaskQueue.hpp"
#include "Logger.hpp"
#include <atomic>

namespace mpc::concurrency
{
    struct LogSysExRamFullTask
    {
        void operator()() const
        {
            MLOG("SysEx RAM full");
        }
    };

    class UiTaskQueue
    {
        TaskQueue tasks;
        std::atomic<bool> sysExLogPending{false};

    public:
        bool post(utils::Task &task)
        {
            return tasks.post(task);
        }
        bool post(LogSysExRamFullTask)
        {
            if (sysExLogPending.exchange(true))
            {
                return true;
            }
            utils::Task task(
                [this]
                {
                    LogSysExRamFullTask{}();
                    sysExLogPending.store(false);
                });
            if (tasks.post(task))
            {
                return true;
            }
            sysExLogPending.store(false);
            return false;
        }
        void drain()
        {
            tasks.drain();
        }
    };
} // namespace mpc::concurrency
