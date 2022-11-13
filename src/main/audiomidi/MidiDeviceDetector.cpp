#include "MidiDeviceDetector.hpp"

#include "RtMidi.h"
#include "Mpc.hpp"
#include "Paths.hpp"

#include "nvram/MidiMappingPersistence.hpp"
#include "file/File.hpp"
#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <vector>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

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
                    if (name.find("MPD") != std::string::npos)
                    {
                        moduru::file::File f(Paths::midiControllerPresetsPath() + "MPD218", nullptr);
                        auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
                        vmpcMidiScreen->realtimeSwitchCommands.clear();
                        mpc::nvram::MidiMappingPersistence::loadMappingFromFile(f, vmpcMidiScreen->realtimeSwitchCommands);
                        auto knownControllerDetectedScreen = mpc.screens->get<VmpcKnownControllerDetectedScreen>("vmpc-known-controller-detected");
                        knownControllerDetectedScreen->controllerName = "MPD218";
                        mpc.getLayeredScreen()->openScreen("vmpc-known-controller-detected");
                    }
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
