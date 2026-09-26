#include "MidiDeviceDetector.hpp"

#include "Mpc.hpp"
#include "Paths.hpp"

#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include <RtMidi.h>

#include "Logger.hpp"

#include <algorithm>
#include <vector>

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
    RtMidiIn createMidiInForDeviceDetection()
    {
        std::vector<RtMidi::Api> compiledApis;
        RtMidi::getCompiledApi(compiledApis);

#if defined(__linux__) && !defined(__ANDROID__)
        for (const auto api : {RtMidi::LINUX_ALSA, RtMidi::UNIX_JACK})
        {
            if (std::find(compiledApis.begin(), compiledApis.end(), api) ==
                compiledApis.end())
            {
                continue;
            }

            try
            {
                return RtMidiIn(api);
            }
            catch (const RtMidiError &)
            {
                // Try the next preferred API.
            }
        }
#endif

        // Automatic selection now falls back to Dummy when no inputs exist.
        // Keep a real backend alive so polling can discover later connections.
        for (const auto api : compiledApis)
        {
            if (api != RtMidi::RTMIDI_DUMMY)
            {
                return RtMidiIn(api);
            }
        }
        return RtMidiIn(RtMidi::RTMIDI_DUMMY);
    }

#ifdef __APPLE__
    bool canCreateCoreMidiClient()
    {
        MIDIClientRef client = 0;
        const auto status = MIDIClientCreate(CFSTR("vMPC MidiDeviceDetector"),
                                             nullptr, nullptr, &client);

        if (status != noErr)
        {
            MLOG(
                "MidiDeviceDetector: CoreMIDI client preflight failed with "
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
    startWorker(
        [&mpc, this]
        {
#ifdef __APPLE__
            if (!canCreateCoreMidiClient())
            {
                return;
            }
#endif

            auto rtMidiIn = createMidiInForDeviceDetection();

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

                        mpc_fs::path path;
                        const auto knownControllerDetectedScreen =
                            mpc.screens->get<
                                ScreenId::VmpcKnownControllerDetectedScreen>();

                        if (name.find("MPD16") != std::string::npos)
                        {
                            path = mpc.paths->getDocuments()
                                       ->midiControlPresetsPath() /
                                   "MPD16.json";
                            knownControllerDetectedScreen->setControllerName(
                                "MPD16");
                        }
                        else if (name.find("MPD218") != std::string::npos)
                        {
                            path = mpc.paths->getDocuments()
                                       ->midiControlPresetsPath() /
                                   "MPD218.json";
                            knownControllerDetectedScreen->setControllerName(
                                "MPD218");
                        }
                        else if (name.find("iRig PADS") != std::string::npos)
                        {
                            path = mpc.paths->getDocuments()
                                       ->midiControlPresetsPath() /
                                   "iRig_PADS.json";
                            knownControllerDetectedScreen->setControllerName(
                                "iRig_PADS");
                        }
                        else if (name.find("MPC Studio") != std::string::npos)
                        {
                            path = mpc.paths->getDocuments()
                                       ->midiControlPresetsPath() /
                                   "MPC_Studio.json";
                            knownControllerDetectedScreen->setControllerName(
                                "MPC_Studio");
                        }

                        const auto presetExistsRes =
                            path.empty() ? mpc_fs::result<bool>{false}
                                         : mpc_fs::exists(path);

                        if (!path.empty() && presetExistsRes &&
                            *presetExistsRes)
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
        });
}

void MidiDeviceDetector::startWorker(std::function<void()> work)
{
    // The worker never takes this mutex, so joining here cannot deadlock it.
    std::lock_guard lock(lifecycleMutex);
    if (running.load())
    {
        return;
    }
    if (pollThread && pollThread->joinable())
    {
        pollThread->join();
    }

    running.store(true);
    try
    {
        pollThread = std::make_unique<std::thread>(
            [this, work = std::move(work)]
            {
                try
                {
                    work();
                }
                catch (const RtMidiError &error)
                {
                    MLOG("MidiDeviceDetector stopped: " + error.getMessage());
                }
                running.store(false);
            });
    }
    catch (...)
    {
        running.store(false);
        throw;
    }
}

void MidiDeviceDetector::stop()
{
    std::lock_guard lock(lifecycleMutex);
    running.store(false);
}

MidiDeviceDetector::~MidiDeviceDetector()
{
    std::lock_guard lock(lifecycleMutex);
    running.store(false);
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
