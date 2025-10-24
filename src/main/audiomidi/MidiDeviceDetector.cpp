#include "MidiDeviceDetector.hpp"

#include "Mpc.hpp"
#include "Paths.hpp"

#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include <RtMidi.h>

#ifdef _WIN32
#include <Windows.hpp>
#endif

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

void MidiDeviceDetector::start(mpc::Mpc &mpc)
{
    running = true;
    pollThread = new std::thread([&mpc, this]
                                 {
                                     std::shared_ptr<RtMidiIn> rtMidiIn;

                                     // RtMidiIn instantiation throws an exception when building an LV2 in
                                     // several CI/CD build environments, so when we catch an exception,
                                     // the thread's work is done.
                                     try
                                     {
                                         rtMidiIn = std::make_shared<RtMidiIn>();
                                     }
                                     catch (const RtMidiError &e)
                                     {
                                         return;
                                     }

                                     lower_my_priority();

                                     while (running)
                                     {
                                         std::set<std::string> allCurrentNames;

                                         if (mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-continue-previous-session")
                                         {
                                             std::this_thread::sleep_for(std::chrono::milliseconds(500));
                                             continue;
                                         }

                                         for (int i = 0; i < rtMidiIn->getPortCount(); i++)
                                         {
                                             auto name = rtMidiIn->getPortName(i);
                                             allCurrentNames.emplace(name);
                                         }

                                         for (auto &name : allCurrentNames)
                                         {
                                             if (deviceNames.emplace(name).second)
                                             {
                                                 MLOG("A new MIDI device was connected: " + name);

                                                 fs::path path;
                                                 auto knownControllerDetectedScreen = mpc.screens->get<VmpcKnownControllerDetectedScreen>();

                                                 if (name.find("MPD16") != std::string::npos)
                                                 {
                                                     path = mpc.paths->midiControlPresetsPath() / "MPD16.vmp";
                                                     knownControllerDetectedScreen->controllerName = "MPD16";
                                                 }
                                                 else if (name.find("MPD218") != std::string::npos)
                                                 {
                                                     path = mpc.paths->midiControlPresetsPath() / "MPD218.vmp";
                                                     knownControllerDetectedScreen->controllerName = "MPD218";
                                                 }
                                                 else if (name.find("iRig PADS") != std::string::npos)
                                                 {
                                                     path = mpc.paths->midiControlPresetsPath() / "iRig_PADS.vmp";
                                                     knownControllerDetectedScreen->controllerName = "iRig_PADS";
                                                 }
                                                 else if (name.find("MPC Studio") != std::string::npos)
                                                 {
                                                     path = mpc.paths->midiControlPresetsPath() / "MPC_Studio.vmp";
                                                     knownControllerDetectedScreen->controllerName = "MPC_Studio";
                                                 }

                                                 if (!path.empty() && fs::exists(path))
                                                 {
                                                     auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>();
                                                     mpc.getDisk()->readMidiControlPreset(path, vmpcMidiScreen->switchToPreset);
                                                     mpc.getLayeredScreen()->openScreen<VmpcKnownControllerDetectedScreen>();
                                                 }
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

    if (pollThread && pollThread->joinable())
    {
        pollThread->join();
    }

    delete pollThread;
}

auto MidiDeviceDetector::lower_my_priority() -> bool
{
#ifdef _WIN32
    int priority{GetThreadPriority(GetCurrentThread())};
    return priority != THREAD_PRIORITY_ERROR_RETURN && priority > THREAD_PRIORITY_IDLE && SetThreadPriority(GetCurrentThread(), priority > THREAD_PRIORITY_HIGHEST ? THREAD_PRIORITY_HIGHEST : priority > THREAD_PRIORITY_ABOVE_NORMAL ? THREAD_PRIORITY_ABOVE_NORMAL
                                                                                                                                                                                           : priority > THREAD_PRIORITY_NORMAL         ? THREAD_PRIORITY_NORMAL
                                                                                                                                                                                           : priority > THREAD_PRIORITY_BELOW_NORMAL   ? THREAD_PRIORITY_BELOW_NORMAL
                                                                                                                                                                                           : priority > THREAD_PRIORITY_LOWEST         ? THREAD_PRIORITY_LOWEST
                                                                                                                                                                                                                                       : THREAD_PRIORITY_IDLE) != 0;
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
