#include "Mpc.hpp"

#include <mpc_fs.hpp>

#include "DemoFiles.hpp"

#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "performance/PerformanceManager.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include "Paths.hpp"
#include "nvram/NvRam.hpp"
#include "input/midi/legacy/LegacyOnDiskPresetConvertor.hpp"

#include "disk/AbstractDisk.hpp"

#include "engine/EngineHost.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiDeviceDetector.hpp"
#include "audiomidi/MidiOutput.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Clock.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/Screens.hpp"

#include "MpcResourceUtil.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "input/midi/MidiControlPresetV3.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientExtendedMidiController.hpp"

#include "Logger.hpp"

#include <nlohmann/json.hpp>

#include <string>

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::hardware;

Mpc::Mpc()
{
    paths = std::make_shared<Paths>();
    clock = std::make_shared<Clock>();
}

void Mpc::init()
{
    const std::vector requiredPaths{
        paths->getDocuments()->appDocumentsPath(),
        paths->configPath(),
        paths->getDocuments()->storesPath(),
        paths->getDocuments()->defaultLocalVolumePath(),
        paths->getDocuments()->recordingsPath(),
        paths->getDocuments()->autoSavePath()};

    for (auto &p : requiredPaths)
    {
        if (!fs::exists(p))
        {
            fs::create_directories(p);
        }
    }

    fs::create_directories(paths->getDocuments()->demoDataPath() / "TEST1");
    fs::create_directories(paths->getDocuments()->demoDataPath() / "TEST2");
    fs::create_directories(paths->getDocuments()->demoDataPath() / "TRAIN1");
    fs::create_directories(paths->getDocuments()->demoDataPath() / "RESIST");

    for (const auto &demo_file : demo_files)
    {
        const auto dst = paths->getDocuments()->demoDataPath() / demo_file;
        const bool should_update =
            !fs::exists(dst) ||
            std::find(always_update_demo_files.begin(),
                      always_update_demo_files.end(),
                      demo_file) != always_update_demo_files.end();

        if (should_update)
        {
            const auto data =
                MpcResourceUtil::get_resource_data("demodata/" + demo_file);
            set_file_data(dst, data);
        }
    }

    fs::create_directories(paths->getDocuments()->midiControlPresetsPath());

    const std::vector<std::string> factory_midi_control_presets{
        "MPD16.json", "MPD218.json", "iRig_PADS.json", "MPC_Studio.json"};

    for (auto &preset : factory_midi_control_presets)
    {
        const auto data =
            MpcResourceUtil::get_resource_data("midicontrolpresets/" + preset);

        if (!fs::exists(paths->getDocuments()->midiControlPresetsPath() /
                        preset) ||
            fs::file_size(paths->getDocuments()->midiControlPresetsPath() /
                          preset) != data.size())
        {
            set_file_data(
                paths->getDocuments()->midiControlPresetsPath() / preset, data);
        }
    }

    input::midi::legacy::convertOnDiskLegacyPresets(
        paths->getDocuments()->midiControlPresetsPath());

    input::midi::legacy::convertOnDiskLegacyPreset(
        paths->legacyActiveMidiControlPresetPath(),
        paths->getDocuments()->activeMidiControlPresetPath());

    Logger::l.setPath(paths->getDocuments()->logFilePath().string());

    padAndButtonKeyboard = std::make_shared<input::PadAndButtonKeyboard>(*this);

    diskController = std::make_unique<disk::DiskController>(*this);

    hardware = std::make_shared<Hardware>();

    midiOutput = std::make_shared<audiomidi::MidiOutput>();

    layeredScreen = std::make_shared<LayeredScreen>(*this);

    performanceManager = std::make_shared<performance::PerformanceManager>(
        layeredScreen->postToUiThread);

    sampler = std::make_shared<Sampler>(
        *this,
        GetProgramFn(
            [manager =
                 performanceManager.get()](const ProgramIndex programIndex)
            {
                return manager->getSnapshot().getProg(programIndex);
            }),
        [&](performance::PerformanceMessage &&m)
        {
            performanceManager->enqueue(std::move(m));
        });
    MLOG("Sampler created");

    screens = std::make_shared<Screens>(*this);

    eventHandler = std::make_shared<audiomidi::EventHandler>(*this);
    MLOG("EventHandler created");

    clientEventController =
        std::make_shared<controller::ClientEventController>(*this);

    engineHost = std::make_shared<engine::EngineHost>(*this);

    MLOG("EngineHost created");

    sequencer = std::make_shared<Sequencer>(
        engineHost->postToAudioThread,
        [&]
        {
            engineHost->flushNoteOffs();
        },
        clock, layeredScreen,
        [&]
        {
            return screens;
        },
        &engineHost->getVoices(),
        [&]
        {
            return engineHost->getAudioServer()->isRunning();
        },
        hardware,
        [&]
        {
            engineHost->stopBouncing();
        },
        [&]
        {
            return engineHost->isBouncing();
        },
        [&]
        {
            return clientEventController->isEraseButtonPressed();
        },
        performanceManager, sampler, eventHandler,
        [&]
        {
            return clientEventController->isSixteenLevelsEnabled();
        });
    MLOG("Sequencer created");

    // We create all screens once so they're all cached in mpc::lcdgui::Screens,
    // avoiding memory allocations and I/O on the audio thread.
    screens->createAndCacheAllScreens();

    sequencer->init();
    MLOG("Sequencer initialized");

    engineHost->start();
    MLOG("EngineHost started");

    clientEventController->init();

    // This needs to happen before the sampler initializes initMasterPadAssign
    // which we do in Sampler::init()
    nvram::NvRam::loadVmpcSettings(*this);

    sampler->init();
    MLOG("Sampler initialized");

    nvram::NvRam::loadUserScreenValues(*this);

    if (const auto p = paths->getDocuments()->activeMidiControlPresetPath();
        fs::exists(p))
    {
        const auto data = get_file_data(p);
        const auto dataJson = json::parse(data);
        input::midi::from_json(
            dataJson, *clientEventController->getClientMidiEventController()
                           ->getExtendedController()
                           ->getActivePreset());
    }

    midiDeviceDetector = std::make_shared<audiomidi::MidiDeviceDetector>();

    MLOG("Mpc is ready");

    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    startMidiDeviceDetector();
    getEngineHost()->getAudioServer()->start();

    autoSave = std::make_unique<AutoSave>();
}

std::shared_ptr<Hardware> Mpc::getHardware()
{
    return hardware;
}

void Mpc::dispatchHostInput(const input::HostInputEvent &hostEvent) const
{
    clientEventController->dispatchHostInput(hostEvent);
}

std::shared_ptr<Sequencer> Mpc::getSequencer()
{
    return sequencer;
}

std::shared_ptr<Sampler> Mpc::getSampler()
{
    return sampler;
}

std::shared_ptr<engine::EngineHost> Mpc::getEngineHost()
{
    return engineHost;
}

std::shared_ptr<audiomidi::EventHandler> Mpc::getEventHandler()
{
    return eventHandler;
}

std::shared_ptr<LayeredScreen> Mpc::getLayeredScreen()
{
    return layeredScreen;
}

std::shared_ptr<ScreenComponent> Mpc::getScreen() const
{
    return layeredScreen->getCurrentScreen();
}

std::shared_ptr<disk::AbstractDisk> Mpc::getDisk() const
{
    return diskController->getActiveDisk();
}

std::vector<std::shared_ptr<disk::AbstractDisk>> Mpc::getDisks() const
{
    return diskController->getDisks();
}

std::vector<char> Mpc::akaiAsciiChar{
    ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '-', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9',  '@', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L',  'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y',  'Z', '_', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k',  'l', 'm', 'n', 'o', 'p', 'q',
    'r', 's', 't', 'u', 'v', 'w', 'x',  'y', 'z', '{', '}'};
std::vector<std::string> Mpc::akaiAscii{
    " ", "!", "#", "$", "%", "&", "'", "(", ")", "-", "0", "1", "2",
    "3", "4", "5", "6", "7", "8", "9", "@", "A", "B", "C", "D", "E",
    "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "a", "b", "c", "d",
    "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q",
    "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "}"};

std::shared_ptr<audiomidi::MidiOutput> Mpc::getMidiOutput()
{
    return midiOutput;
}

disk::DiskController *Mpc::getDiskController() const
{
    return diskController.get();
}

Mpc::~Mpc()
{
    if (autoSave)
    {
        autoSave->interruptRestorationIfStillOngoing();
    }

    nvram::NvRam::saveUserScreenValues(*this);
    nvram::NvRam::saveVmpcSettings(*this);
    if (engineHost)
    {
        engineHost->destroyServices();
    }
}

void Mpc::panic() const
{
    // TODO Anything we should do in the performanceManager? Probably not,
    // because panic is meant to help out other devices, i.e. things connected
    // to the MPC2000XL's MIDI output.
    midiOutput->panic();
}

std::shared_ptr<Clock> Mpc::getClock()
{
    return clock;
}

void Mpc::setPluginModeEnabled(const bool b)
{
    pluginModeEnabled = b;
}

bool Mpc::isPluginModeEnabled() const
{
    return pluginModeEnabled;
}

std::weak_ptr<performance::PerformanceManager> Mpc::getPerformanceManager()
{
    return performanceManager;
}

void Mpc::startMidiDeviceDetector()
{
    midiDeviceDetector->start(*this);
}

std::shared_ptr<input::PadAndButtonKeyboard> Mpc::getPadAndButtonKeyboard()
{
    return padAndButtonKeyboard;
}

AutoSave *Mpc::getAutoSave() const
{
    return autoSave.get();
}