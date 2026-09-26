#pragma once

#include <set>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <mutex>

namespace mpc
{
    class Mpc;
}

namespace mpc::audiomidi
{
    class MidiDeviceDetector
    {
    public:
        void start(Mpc &);
        void stop();
        ~MidiDeviceDetector();

    private:
        friend struct MidiDeviceDetectorTestAccess;
        // A private worker seam lets lifecycle tests avoid OS MIDI services.
        void startWorker(std::function<void()> work);
        std::mutex lifecycleMutex;
        std::atomic<bool> running{false};
        std::unique_ptr<std::thread> pollThread;
        std::set<std::string> deviceNames;
        auto lower_my_priority() -> bool;
    };
} // namespace mpc::audiomidi
