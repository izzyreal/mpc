#include "MidiDeviceDetector.hpp"

#include "Mpc.hpp"
#include "Paths.hpp"

#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <RtMidi.h>

#include "Logger.hpp"

#ifdef __APPLE__
#include <CoreMIDI/CoreMIDI.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

namespace
{
#ifdef __APPLE__
    bool canCreateCoreMidiClient()
    {
        MIDIClientRef client = 0;
        const auto status = MIDIClientCreate(CFSTR("vMPC MidiDeviceDetector"),
                                             nullptr, nullptr, &client);

        if (status != noErr)
        {
            MLOG("MidiDeviceDetector: CoreMIDI client preflight failed with "
                 "status " +
                 std::to_string(static_cast<int>(status)) +
                 ", skipping MIDI device detection.");
            return false;
        }

        MIDIClientDispose(client);
        return true;
    }
#endif
} // namespace

void MidiDeviceDetector::start(Mpc &mpc)
{
    if (running.load())
    {
        return;
    }

    running.store(true);

    pollThread = std::make_unique<std::thread>(
        [&mpc, this]
        {
            try
            {
#ifdef __APPLE__
                if (!canCreateCoreMidiClient())
                {
                    running.store(false);
                    return;
                }
#endif

                RtMidiIn rtMidiIn;

                if (!lower_my_priority())
                {
                    MLOG("MidiDeviceDetector failed to lower its priority!");
                }

                while (running.load())
                {
                    std::set<std::string> allCurrentNames;

                    for (int i = 0; i < rtMidiIn.getPortCount(); i++)
                    {
                        auto name = rtMidiIn.getPortName(i);
                        allCurrentNames.emplace(name);
                    }

                    for (auto &name : allCurrentNames)
                    {
                        if (deviceNames.emplace(name).second)
                        {
                            MLOG("A new MIDI device was connected: " + name);

                            fs::path path;
                            const auto knownControllerDetectedScreen =
                                mpc.screens->get<
                                    ScreenId::
                                        VmpcKnownControllerDetectedScreen>();

                            if (name.find("MPD16") != std::string::npos)
                            {
                                path = mpc.paths->getDocuments()
                                           ->midiControlPresetsPath() /
                                       "MPD16.json";
                                knownControllerDetectedScreen
                                    ->setControllerName("MPD16");
                            }
                            else if (name.find("MPD218") != std::string::npos)
                            {
                                path = mpc.paths->getDocuments()
                                           ->midiControlPresetsPath() /
                                       "MPD218.json";
                                knownControllerDetectedScreen
                                    ->setControllerName("MPD218");
                            }
                            else if (name.find("iRig PADS") !=
                                     std::string::npos)
                            {
                                path = mpc.paths->getDocuments()
                                           ->midiControlPresetsPath() /
                                       "iRig_PADS.json";
                                knownControllerDetectedScreen
                                    ->setControllerName("iRig_PADS");
                            }
                            else if (name.find("MPC Studio") !=
                                     std::string::npos)
                            {
                                path = mpc.paths->getDocuments()
                                           ->midiControlPresetsPath() /
                                       "MPC_Studio.json";
                                knownControllerDetectedScreen
                                    ->setControllerName("MPC_Studio");
                            }

                            if (!path.empty() && fs::exists(path))
                            {
                                auto layeredScreen = mpc.getLayeredScreen();
                                layeredScreen->postToUiThread(utils::Task(
                                    [layeredScreen]
                                    {
                                        layeredScreen->openScreenById(
                                            ScreenId::
                                                VmpcKnownControllerDetectedScreen);
                                    }));
                            }
                        }
                    }

                    deviceNames = allCurrentNames;

                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
            catch (const RtMidiError &)
            {
                // RtMidiIn instantiation throws an exception when building an LV2
                // in several CI/CD build environments, so when we catch an
                // exception, the thread's work is done.
            }
        });
}

void MidiDeviceDetector::stop()
{
    running.store(false);
}

MidiDeviceDetector::~MidiDeviceDetector()
{
    stop();

    if (pollThread && pollThread->joinable())
    {
        pollThread->join();
    }
}

auto MidiDeviceDetector::lower_my_priority() -> bool
{
#ifdef _WIN32
    int priority{GetThreadPriority(GetCurrentThread())};
    return priority != THREAD_PRIORITY_ERROR_RETURN &&
           priority > THREAD_PRIORITY_IDLE &&
           SetThreadPriority(
               GetCurrentThread(),
               priority > THREAD_PRIORITY_HIGHEST ? THREAD_PRIORITY_HIGHEST
               : priority > THREAD_PRIORITY_ABOVE_NORMAL
                   ? THREAD_PRIORITY_ABOVE_NORMAL
               : priority > THREAD_PRIORITY_NORMAL ? THREAD_PRIORITY_NORMAL
               : priority > THREAD_PRIORITY_BELOW_NORMAL
                   ? THREAD_PRIORITY_BELOW_NORMAL
               : priority > THREAD_PRIORITY_LOWEST ? THREAD_PRIORITY_LOWEST
                                                   : THREAD_PRIORITY_IDLE) != 0;
#else
    int policy;
    sched_param params;
    if (pthread_getschedparam(pthread_self(), &policy, &params) == 0)
    {
        int const min_value{sched_get_priority_min(policy)};
        if (min_value != -1)
        {
            if (params.sched_priority > min_value)
            {
                --params.sched_priority;
                if (pthread_setschedparam(pthread_self(), policy, &params) !=
                    -1)
                {
                    return true;
                }
            }
        }
    }

    return false;
#endif
}
