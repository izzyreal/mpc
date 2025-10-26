#include <mpc_fs.hpp>

#include "Mpc.hpp"

#include "DemoFiles.hpp"

#include "controller/ClientMidiEventController.hpp"
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
#include "engine/PreviewSoundPlayer.hpp"
#include "engine/Drum.hpp"

#include "hardware/Hardware.hpp"

#include "lcdgui/Screens.hpp"

#include "MpcResourceUtil.hpp"
#include "hardware/ComponentId.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "controller/ClientEventController.hpp"

#include <string>

using namespace mpc;
using namespace mpc::lcdgui;

Mpc::Mpc()
{
    paths = std::make_shared<mpc::Paths>();
    clock = std::make_shared<mpc::sequencer::Clock>();
}

void Mpc::init()
{
    std::vector<fs::path> requiredPaths{
        paths->appDocumentsPath(),
        paths->configPath(),
        paths->storesPath(),
        paths->defaultLocalVolumePath(),
        paths->recordingsPath(),
        paths->autoSavePath()};

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
        const bool should_update = !fs::exists(dst) || std::find(always_update_demo_files.begin(),
                                                                 always_update_demo_files.end(),
                                                                 demo_file) != always_update_demo_files.end();

        if (should_update)
        {
            const auto data = mpc::MpcResourceUtil::get_resource_data("demodata/" + demo_file);
            set_file_data(dst, data);
        }
    }

    fs::create_directories(paths->midiControlPresetsPath());

    const std::vector<std::string> factory_midi_control_presets{"MPD16.vmp", "MPD218.vmp", "iRig_PADS.vmp"};

    for (auto &preset : factory_midi_control_presets)
    {
        const auto data = mpc::MpcResourceUtil::get_resource_data("midicontrolpresets/" + preset);

        if (!fs::exists(paths->midiControlPresetsPath() / preset) || fs::file_size(paths->midiControlPresetsPath() / preset) != data.size())
        {
            set_file_data(paths->midiControlPresetsPath() / preset, data);
        }
    }

    mpc::Logger::l.setPath(paths->logFilePath().string());

    padAndButtonKeyboard = std::make_shared<mpc::input::PadAndButtonKeyboard>(*this);

    diskController = std::make_unique<mpc::disk::DiskController>(*this);

    nvram::MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(*this);

    sequencer = std::make_shared<mpc::sequencer::Sequencer>(*this);
    MLOG("Sequencer created");

    hardware = std::make_shared<hardware::Hardware>();

    sampler = std::make_shared<mpc::sampler::Sampler>(*this);
    MLOG("Sampler created");

    midiOutput = std::make_shared<audiomidi::MidiOutput>();

    layeredScreen = std::make_shared<lcdgui::LayeredScreen>(*this);

    screens = std::make_shared<Screens>(*this);
    // We create all screens once so they're all cached in mpc::lcdgui::Screens,
    // avoiding memory allocations and I/O on the audio thread.
    screens->createAndCacheAllScreens();

    eventHandler = std::make_shared<mpc::audiomidi::EventHandler>(*this);
    MLOG("EventHandler created");

    clientEventController = std::make_shared<mpc::controller::ClientEventController>(*this);
    clientEventController->init();

    /*
     * AudioMidiServices requires sequencer to exist.
     */
    audioMidiServices = std::make_shared<mpc::audiomidi::AudioMidiServices>(*this);
    MLOG("AudioMidiServices created");

    sequencer->init();
    MLOG("Sequencer initialized");

    audioMidiServices->start();
    MLOG("AudioMidiServices started");

    // This needs to happen before the sampler initializes initMasterPadAssign
    // which we do in Sampler::init()
    mpc::nvram::NvRam::loadVmpcSettings(*this);

    sampler->init();
    MLOG("Sampler initialized");

    mpc::nvram::NvRam::loadUserScreenValues(*this);

    mpc::nvram::MidiControlPersistence::restoreLastState(*this);

    midiDeviceDetector = std::make_shared<audiomidi::MidiDeviceDetector>();

    MLOG("Mpc is ready");

    layeredScreen->openScreen<SequencerScreen>();
}

std::shared_ptr<hardware::Hardware> Mpc::getHardware()
{
    return hardware;
}

void Mpc::dispatchHostInput(const mpc::input::HostInputEvent &hostEvent)
{
    clientEventController->dispatchHostInput(hostEvent);
}

std::shared_ptr<mpc::sequencer::Sequencer> Mpc::getSequencer()
{
    return sequencer;
}

std::shared_ptr<sampler::Sampler> Mpc::getSampler()
{
    return sampler;
}

mpc::engine::Drum &Mpc::getDrum(int i)
{
    return audioMidiServices->getDrum(i);
}

mpc::engine::PreviewSoundPlayer &Mpc::getBasicPlayer()
{
    return audioMidiServices->getBasicPlayer();
}

std::shared_ptr<audiomidi::AudioMidiServices> Mpc::getAudioMidiServices()
{
    return audioMidiServices;
}

std::shared_ptr<audiomidi::EventHandler> Mpc::getEventHandler()
{
    return eventHandler;
}

std::shared_ptr<lcdgui::LayeredScreen> Mpc::getLayeredScreen()
{
    return layeredScreen;
}

std::shared_ptr<lcdgui::ScreenComponent> Mpc::getScreen()
{
    return layeredScreen->getCurrentScreen();
}

std::shared_ptr<mpc::disk::AbstractDisk> Mpc::getDisk()
{
    return diskController->getActiveDisk();
}

std::vector<std::shared_ptr<mpc::disk::AbstractDisk>> Mpc::getDisks()
{
    return diskController->getDisks();
}

std::vector<char> Mpc::akaiAsciiChar{' ', '!', '#', '$', '%', '&', '\'', '(', ')', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '}'};
std::vector<std::string> Mpc::akaiAscii{" ", "!", "#", "$", "%", "&", "'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "}"};

std::shared_ptr<audiomidi::MidiOutput> Mpc::getMidiOutput()
{
    return midiOutput;
}

mpc::disk::DiskController *Mpc::getDiskController()
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
        sequencer->stop();
    }

    mpc::nvram::MidiControlPersistence::saveCurrentState(*this);
    mpc::nvram::NvRam::saveUserScreenValues(*this);
    mpc::nvram::NvRam::saveVmpcSettings(*this);

    if (layeredScreen)
    {
        layeredScreen.reset();
    }

    if (audioMidiServices)
    {
        audioMidiServices->destroyServices();
    }
}

void Mpc::panic()
{
    sampler->clearAllProgramPadPressRegistries();
    sequencer->getNoteEventStore().clearPlayAndRecordStore();
    midiOutput->panic();
    eventHandler->clearTransposeCache();
    clientEventController->getClientMidiEventController()->getSoundGeneratorController()->clearNoteEventStore();
}

std::shared_ptr<mpc::sequencer::Clock> Mpc::getClock()
{
    return clock;
}

void Mpc::setPluginModeEnabled(bool b)
{
    pluginModeEnabled = b;
}

bool Mpc::isPluginModeEnabled()
{
    return pluginModeEnabled;
}

void Mpc::startMidiDeviceDetector()
{
    midiDeviceDetector->start(*this);
}

std::shared_ptr<mpc::input::PadAndButtonKeyboard> Mpc::getPadAndButtonKeyboard()
{
    return padAndButtonKeyboard;
}
