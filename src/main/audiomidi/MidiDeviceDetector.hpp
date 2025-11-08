#pragma once

#include <set>
#include <string>
#include <thread>
#include <atomic>
#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::audiomidi
{
    class MidiDeviceDetector
    {
    public:
        void start(mpc::Mpc &);
        void stop();
        ~MidiDeviceDetector();

    private:
        std::atomic<bool> running{false};
        std::unique_ptr<std::thread> pollThread;
        std::set<std::string> deviceNames;
        auto lower_my_priority() -> bool;
    };
} // namespace mpc::audiomidi
