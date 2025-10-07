#include <mpc_fs.hpp>

#include "Mpc.hpp"

#include <lcdgui/ScreenComponent.hpp>

#include "Paths.hpp"
#include <nvram/NvRam.hpp>
#include <nvram/MidiControlPersistence.hpp>

#include <disk/AbstractDisk.hpp>

#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MidiDeviceDetector.hpp>
#include <audiomidi/MidiInput.hpp>
#include <audiomidi/MidiOutput.hpp>

#include <sampler/Sampler.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Clock.hpp>
#include "engine/PreviewSoundPlayer.hpp"
#include "engine/Drum.hpp"

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/Led.hpp>

#include <lcdgui/Screens.hpp>

#include "MpcResourceUtil.hpp"
#include "inputsystem/Initializer.h"

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
    std::vector<fs::path> requiredPaths {
            paths->appDocumentsPath(),
            paths->configPath(),
            paths->storesPath(),
            paths->defaultLocalVolumePath(),
            paths->recordingsPath(),
            paths->autoSavePath()
    };

    for (auto& p : requiredPaths)
    {
        if (!fs::exists(p))
        {
            fs::create_directories(p);
        }
    }

    const std::vector<std::string> demo_files{ "TEST1/BASIC_KIT.ALL", "TEST1/BASIC_KIT.APS", "TEST1/BASIC_KIT.MID", "TEST1/BASIC_KIT.PGM",
                                               "TEST1/HAT1.SND", "TEST1/KICK1.SND", "TEST1/SNARE4.SND", "TEST2/2PEOPLE2.SND", "TEST2/2PEOPLE3.SND", "TEST2/CLIMAX.SND",
                                               "TEST2/FRUTZLE.ALL", "TEST2/FRUTZLE.APS", "TEST2/FRUTZLE.MID", "TEST2/FRUTZLE.PGM", "TEST2/KICKHAT3.SND",
                                               "TEST2/KICKHAT5.SND", "TEST2/KICK_C_1.SND", "TEST2/KICK_C_2.SND", "TEST2/MOOI.SND", "TEST2/OBOE.SND",
                                               "TEST2/PIAMA2.SND", "TEST2/PIAMA3.SND", "TEST2/RIDE.SND", "TEST2/RIDE1.SND", "TEST2/SOLOBASS.SND",
                                               "TEST2/WOOSH.SND" };

    if (!fs::exists(paths->demoDataPath()))
    {
        fs::create_directories(paths->demoDataPath() / "TEST1");
        fs::create_directories(paths->demoDataPath() / "TEST2");
        for (auto& demo_file : demo_files)
        {
            const auto data = mpc::MpcResourceUtil::get_resource_data("demodata/" + demo_file);
            set_file_data(paths->demoDataPath() / demo_file, data);
        }
    }

    fs::create_directories(paths->midiControlPresetsPath());

    const std::vector<std::string> factory_midi_control_presets{"MPD16.vmp", "MPD218.vmp", "iRig_PADS.vmp" };

    for (auto& preset : factory_midi_control_presets)
    {
        const auto data = mpc::MpcResourceUtil::get_resource_data("midicontrolpresets/" + preset);

        if (!fs::exists(paths->midiControlPresetsPath() / preset) || fs::file_size(paths->midiControlPresetsPath() / preset) != data.size())
        {
            set_file_data(paths->midiControlPresetsPath() / preset, data);
        }
    }

    mpc::Logger::l.setPath(paths->logFilePath().string());

    hardware = std::make_shared<hardware::Hardware>(*this);

	diskController = std::make_unique<mpc::disk::DiskController>(*this);

    nvram::MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(*this);

    sequencer = std::make_shared<mpc::sequencer::Sequencer>(*this);
	MLOG("Sequencer created");

	sampler = std::make_shared<mpc::sampler::Sampler>(*this);
	MLOG("Sampler created");

    midiInputs = { new mpc::audiomidi::MidiInput(*this, 0), new mpc::audiomidi::MidiInput(*this, 1) };

    midiOutput = std::make_shared<audiomidi::MidiOutput>();

    screens = std::make_shared<Screens>(*this);

    layeredScreen = std::make_shared<lcdgui::LayeredScreen>(*this);

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

	eventHandler = std::make_shared<mpc::audiomidi::EventHandler>(*this);
	MLOG("Eeventhandler created");

	controls = std::make_shared<controls::Controls>(*this);

	mpc::nvram::NvRam::loadUserScreenValues(*this);

    // We fetch all screens once so they're all cached in Screens,
    // avoiding memory allocations on the audio thread.
	for (auto& screenName : screenNames)
        screens->get<ScreenComponent>(screenName);

  mpc::nvram::MidiControlPersistence::restoreLastState(*this);

  layeredScreen->openScreen("sequencer");

  midiDeviceDetector = std::make_shared<audiomidi::MidiDeviceDetector>();

	MLOG("Mpc is ready");

    mpc::inputsystem::Initializer::init(inputMapper, inputController);
}

std::shared_ptr<controls::Controls> Mpc::getControls()
{
	return controls;
}

std::shared_ptr<hardware::Hardware> Mpc::getHardware()
{
	return hardware;
}

std::shared_ptr<mpc::sequencer::Sequencer> Mpc::getSequencer()
{
    return sequencer;
}

std::shared_ptr<sampler::Sampler> Mpc::getSampler()
{
    return sampler;
}

mpc::engine::Drum& Mpc::getDrum(int i)
{
	return audioMidiServices->getDrum(i);
}

mpc::engine::PreviewSoundPlayer& Mpc::getBasicPlayer()
{
	return audioMidiServices->getBasicPlayer();
}

std::shared_ptr<audiomidi::AudioMidiServices> Mpc::getAudioMidiServices()
{
    return audioMidiServices;
}

std::shared_ptr<audiomidi::EventHandler> Mpc::getEventHandler() {
	return eventHandler;
}

std::shared_ptr<lcdgui::LayeredScreen> Mpc::getLayeredScreen() {
	return layeredScreen;
}

std::shared_ptr<lcdgui::ScreenComponent> Mpc::getActiveControls() {
	return layeredScreen->findScreenComponent();
}

std::shared_ptr<mpc::controls::GlobalReleaseControls> Mpc::getReleaseControls() {
	return controls->getReleaseControls();
}

std::shared_ptr<mpc::disk::AbstractDisk> Mpc::getDisk()
{
	return diskController->getActiveDisk();
}

std::vector<std::shared_ptr<mpc::disk::AbstractDisk>> Mpc::getDisks()
{
    return diskController->getDisks();
}

std::vector<char> Mpc::akaiAsciiChar { ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '}' };
std::vector<std::string> Mpc::akaiAscii { " ", "!", "#", "$", "%", "&", "'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "}" };

std::shared_ptr<audiomidi::MidiOutput> Mpc::getMidiOutput()
{
	return midiOutput;
}

audiomidi::MidiInput* Mpc::getMpcMidiInput(int i)
{
	return midiInputs[i];
}

void Mpc::setBank(int i)
{
	if (i == bank)
		return;

	if (i < 0 || i > 3)
		return;

	bank = i;

	notifyObservers(std::string("bank"));

	hardware->getLed("pad-bank-a")->light(i == 0);
	hardware->getLed("pad-bank-b")->light(i == 1);
	hardware->getLed("pad-bank-c")->light(i == 2);
	hardware->getLed("pad-bank-d")->light(i == 3);
}

int Mpc::getBank()
{
	return bank;
}

void Mpc::setNote(int newNote)
{
    if (newNote < 35 || newNote > 98)
    {
        return;
    }

    note = newNote;
    notifyObservers(std::string("note"));
}

int Mpc::getNote()
{
	return note;
}

int Mpc::getPad()
{
	return pad;
}

std::string Mpc::getPreviousSamplerScreenName()
{
	return previousSamplerScreenName;
}

void Mpc::setPreviousSamplerScreenName(std::string s)
{
	previousSamplerScreenName = s;
}

mpc::disk::DiskController* Mpc::getDiskController()
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

	for (auto& m : midiInputs)
	{
		if (m != nullptr)
			delete m;
	}

	if (layeredScreen)
    {
        layeredScreen.reset();
    }

	if (audioMidiServices)
    {
        audioMidiServices->destroyServices();
    }
}

void Mpc::setPad(unsigned char padIndexWithBank)
{
    if (padIndexWithBank > 63)
    {
        return;
    }

    pad = padIndexWithBank;

    notifyObservers(std::string("pad"));
}

void Mpc::panic()
{
    controls->clearAllPadStates();
    controls->clearStores();
    midiOutput->panic();
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
