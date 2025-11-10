#include "Mpc.hpp"
#include "sequencer/Transport.hpp"

#include <mpc_fs.hpp>

#include "DemoFiles.hpp"

#include "controller/ClientHardwareEventController.hpp"
#include "engine/MixerInterconnection.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include "Paths.hpp"
#include "nvram/NvRam.hpp"
#include "nvram/MidiControlPersistence.hpp"

#include "disk/AbstractDisk.hpp"

#include "audiomidi/AudioMidiServices.hpp"
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
#include "controller/ClientEventController.hpp"

#include <string>

#include "Logger.hpp"

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
    std::vector<fs::path> requiredPaths{
        paths->appDocumentsPath(), paths->configPath(),
        paths->storesPath(),       paths->defaultLocalVolumePath(),
        paths->recordingsPath(),   paths->autoSavePath()};

    for (auto &p : requiredPaths)
    {
        if (!fs::exists(p))
        {
            fs::create_directories(p);
        }
    }

    fs::create_directories(paths->demoDataPath() / "TEST1");
    fs::create_directories(paths->demoDataPath() / "TEST2");
    fs::create_directories(paths->demoDataPath() / "TRAIN1");
    fs::create_directories(paths->demoDataPath() / "RESIST");

    for (const auto &demo_file : demo_files)
    {
        const auto dst = paths->demoDataPath() / demo_file;
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

    fs::create_directories(paths->midiControlPresetsPath());

    const std::vector<std::string> factory_midi_control_presets{
        "MPD16.vmp", "MPD218.vmp", "iRig_PADS.vmp"};

    for (auto &preset : factory_midi_control_presets)
    {
        const auto data =
            MpcResourceUtil::get_resource_data("midicontrolpresets/" + preset);

        if (!fs::exists(paths->midiControlPresetsPath() / preset) ||
            fs::file_size(paths->midiControlPresetsPath() / preset) !=
                data.size())
        {
            set_file_data(paths->midiControlPresetsPath() / preset, data);
        }
    }

    Logger::l.setPath(paths->logFilePath().string());

    padAndButtonKeyboard = std::make_shared<input::PadAndButtonKeyboard>(*this);

    diskController = std::make_unique<disk::DiskController>(*this);

    nvram::MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(*this);

    sampler = std::make_shared<Sampler>(*this);
    MLOG("Sampler created");

    hardware = std::make_shared<Hardware>();

    midiOutput = std::make_shared<audiomidi::MidiOutput>();

    layeredScreen = std::make_shared<LayeredScreen>(*this);

    screens = std::make_shared<Screens>(*this);

    eventHandler = std::make_shared<audiomidi::EventHandler>(*this);
    MLOG("EventHandler created");

    eventRegistry = std::make_shared<eventregistry::EventRegistry>();

    clientEventController =
        std::make_shared<controller::ClientEventController>(*this);
    /*
     * AudioMidiServices requires sequencer to exist.
     */
    audioMidiServices = std::make_shared<audiomidi::AudioMidiServices>(*this);

    MLOG("AudioMidiServices created");

    sequencer = std::make_shared<Sequencer>(
        layeredScreen,
        [&]
        {
            return screens;
        },
        &audioMidiServices->getVoices(),
        [&]
        {
            return audioMidiServices->getAudioServer()->isRunning();
        },
        hardware,
        [&]
        {
            return audioMidiServices->isBouncePrepared();
        },
        [&]
        {
            audioMidiServices->startBouncing();
        },
        [&]
        {
            audioMidiServices->stopBouncing();
        },
        [&]
        {
            return audioMidiServices->isBouncing();
        },
        [&]
        {
            return clientEventController->isEraseButtonPressed();
        },
        eventRegistry, sampler, eventHandler,
        [&]
        {
            return clientEventController->isSixteenLevelsEnabled();
        },
        clock,
        [&]
        {
            return audioMidiServices->getAudioServer()->getSampleRate();
        },
        [&]
        {
            return clientEventController->isRecMainWithoutPlaying();
        },
        [&]
        {
            return clientEventController->clientHardwareEventController
                ->isNoteRepeatLockedOrPressed();
        },
        [&]
        {
            return audioMidiServices->getMixer();
        },
        [&]
        {
            return clientEventController->isFullLevelEnabled();
        },
        [&]() -> std::vector<engine::MixerInterconnection *> &
        {
            return audioMidiServices->getMixerConnections();
            {};
        });
    MLOG("Sequencer created");

    // We create all screens once so they're all cached in mpc::lcdgui::Screens,
    // avoiding memory allocations and I/O on the audio thread.
    screens->createAndCacheAllScreens();

    sequencer->init();
    MLOG("Sequencer initialized");

    audioMidiServices->start();
    MLOG("AudioMidiServices started");

    clientEventController->init();

    // This needs to happen before the sampler initializes initMasterPadAssign
    // which we do in Sampler::init()
    nvram::NvRam::loadVmpcSettings(*this);

    sampler->init();
    MLOG("Sampler initialized");

    nvram::NvRam::loadUserScreenValues(*this);

    nvram::MidiControlPersistence::restoreLastState(*this);

    midiDeviceDetector = std::make_shared<audiomidi::MidiDeviceDetector>();

    MLOG("Mpc is ready");

    layeredScreen->openScreenById(ScreenId::SequencerScreen);
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

std::shared_ptr<audiomidi::AudioMidiServices> Mpc::getAudioMidiServices()
{
    return audioMidiServices;
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
    if (midiDeviceDetector)
    {
        midiDeviceDetector->stop();
    }
    if (sampler)
    {
        sampler->stopAllVoices(0);
    }
    if (sequencer)
    {
        sequencer->getTransport()->stop();
    }

    nvram::MidiControlPersistence::saveCurrentState(*this);
    nvram::NvRam::saveUserScreenValues(*this);
    nvram::NvRam::saveVmpcSettings(*this);

    if (layeredScreen)
    {
        layeredScreen.reset();
    }

    if (audioMidiServices)
    {
        audioMidiServices->destroyServices();
    }
}

void Mpc::panic() const
{
    // TODO Anything we should do in the eventRegistry? Probably not, because
    // panic is meant to help out other devices, i.e. things connected to the
    // MPC2000XL's MIDI output.
    midiOutput->panic();
    eventHandler->clearTransposeCache();
}

std::shared_ptr<Clock> Mpc::getClock()
{
    return clock;
}

void Mpc::setPluginModeEnabled(bool b)
{
    pluginModeEnabled = b;
}

bool Mpc::isPluginModeEnabled() const
{
    return pluginModeEnabled;
}

void Mpc::startMidiDeviceDetector()
{
    midiDeviceDetector->start(*this);
}

std::shared_ptr<input::PadAndButtonKeyboard> Mpc::getPadAndButtonKeyboard()
{
    return padAndButtonKeyboard;
}
