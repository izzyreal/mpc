#include "MidiDeviceDetector.hpp"

#include "RtMidi.h"
#include "Mpc.hpp"
#include "Paths.hpp"

#include "nvram/MidiControlPersistence.hpp"
#include "file/File.hpp"
#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

void MidiDeviceDetector::start(mpc::Mpc &mpc)
{
    running = true;
    pollThread = new std::thread([&mpc, this] {
        auto rtMidiIn = std::make_shared<RtMidiIn>();
        lower_my_priority();
        while (running)
        {
            std::set<std::string> allCurrentNames;

            for (int i = 0; i < rtMidiIn->getPortCount(); i++)
            {
                auto name = rtMidiIn->getPortName(i);
                allCurrentNames.emplace(name);
            }

            for (auto &name: allCurrentNames)
            {
                if (deviceNames.emplace(name).second)
                {
                    if (name.find("MPD") != std::string::npos)
                    {
                        moduru::file::File f(Paths::midiControlPresetsPath() + "MPD218.vmp", {});
                        auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
                        vmpcMidiScreen->realtimeSwitchCommands.clear();
                        mpc::nvram::MidiControlPersistence::loadMappingFromFile(f,
                                                                                vmpcMidiScreen->realtimeSwitchCommands);
                        auto knownControllerDetectedScreen = mpc.screens->get<VmpcKnownControllerDetectedScreen>(
                                "vmpc-known-controller-detected");
                        knownControllerDetectedScreen->controllerName = "MPD218";
                        mpc.getLayeredScreen()->openScreen("vmpc-known-controller-detected");
                    }
                }
            }

            for (auto &name: deviceNames)
            {
                if (allCurrentNames.find(name) == allCurrentNames.end())
                {
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
    running = false;

    while (!pollThread->joinable())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    pollThread->join();

    delete pollThread;
}

auto MidiDeviceDetector::lower_my_priority() -> bool
{
#ifdef _WIN32
    int priority{ GetThreadPriority(GetCurrentThread()) };
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
#else
    int policy;
    sched_param params;
    if (pthread_getschedparam(
            pthread_self(), &policy, &params) == 0)
    {
        int const min_value{sched_get_priority_min(policy)};
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
#endif
}