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
        auto lower_my_priority() -> bool;
    };
}