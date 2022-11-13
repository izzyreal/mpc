#pragma once

#include <set>
#include <string>
#include <thread>

namespace mpc { class Mpc; }

namespace mpc::audiomidi {
    class MidiDeviceDetector {
    public:
        void start(mpc::Mpc&);
        void stop();
        ~MidiDeviceDetector();

    private:
        bool running = false;
        std::thread* pollThread;
        std::set<std::string> deviceNames;
    };
    namespace
    {
#ifdef WIN32
        auto lower_my_priority() -> bool
    {
        int priority { GetThreadPriority(GetCurrentThread()) };
        return priority != THREAD_PRIORITY_ERROR_RETURN
            && priority > THREAD_PRIORITY_IDLE
            && SetThreadPriority(
                GetCurrentThread(),
                priority > THREAD_PRIORITY_HIGHEST
                    ? THREAD_PRIORITY_HIGHEST
                    : priority > THREAD_PRIORITY_ABOVE_NORMAL
                    ? THREAD_PRIORITY_ABOVE_NORMAL
                    : priority > THREAD_PRIORITY_NORMAL
                    ? THREAD_PRIORITY_NORMAL
                    : priority > THREAD_PRIORITY_BELOW_NORMAL
                    ? THREAD_PRIORITY_BELOW_NORMAL
                    : priority > THREAD_PRIORITY_LOWEST
                    ? THREAD_PRIORITY_LOWEST
                    : THREAD_PRIORITY_IDLE)
            != 0;
    }
#else
        auto lower_my_priority() -> bool
        {
            int policy;
            sched_param params;
            if (pthread_getschedparam(
                    pthread_self(), &policy, &params) == 0)
            {
                int const min_value{ sched_get_priority_min(policy) };
                if (min_value != -1)
                {
                    if (params.sched_priority > min_value)
                    {
                        --params.sched_priority;
                        if (pthread_setschedparam(pthread_self(), policy, &params) != -1)
                        {
                            return true;
                        }
                    }
                }
            }

            return false;
        }
#endif
    }
}