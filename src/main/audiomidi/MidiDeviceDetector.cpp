#include "MidiDeviceDetector.hpp"

#include "RtMidi.h"
#include "Mpc.hpp"

#include <vector>

using namespace mpc::audiomidi;

void MidiDeviceDetector::start(mpc::Mpc& mpc)
{
    auto rtMidiIn = std::make_shared<RtMidiIn>();

    running = true;

    pollThread = new std::thread([&mpc, this, rtMidiIn]{
        lower_my_priority();
        running = true;
        while (running) {
            std::set<std::string> allCurrentNames;

            for (int i = 0; i < rtMidiIn->getPortCount(); i++) {
                auto name = rtMidiIn->getPortName(i);
                allCurrentNames.emplace(name);
            }

            for (auto &name: allCurrentNames) {
                if (deviceNames.emplace(name).second) {
                    // TODO handle new MIDI device
                }
            }

            for (auto &name: deviceNames) {
                if (allCurrentNames.find(name) == allCurrentNames.end()) {
                    // TODO handle remove MIDI device
                }
            }

            deviceNames = allCurrentNames;

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
}

void MidiDeviceDetector::stop()
{
    running = false;
}

MidiDeviceDetector::~MidiDeviceDetector()
{
    if (pollThread->joinable())
    {
        pollThread->join();
    }
}
