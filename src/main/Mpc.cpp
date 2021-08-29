#include <mpc_fs.hpp>

#include "Mpc.hpp"

#include <lcdgui/Component.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include "Paths.hpp"
#include <nvram/NvRam.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/SoundLoader.hpp>

#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MpcMidiInput.hpp>

#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcBasicSoundPlayerChannel.hpp>
#include <mpc/MpcMultiMidiSynth.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <midi/core/MidiTransport.hpp>
#include <synth/SynthChannel.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/Led.hpp>

#include <lcdgui/Screens.hpp>

#include <string>

using namespace mpc;
using namespace mpc::lcdgui;

Mpc::Mpc()
{
    std::vector<std::string> requiredPaths {
        Paths::appDocumentsPath(),
        Paths::appConfigHome(),
        Paths::configPath(),
        Paths::storesPath(),
        Paths::defaultLocalVolumePath(),
        Paths::recordingsPath()
    };

    for (auto& p : requiredPaths) {
        if (!fs::exists(p))
            fs::create_directories(p);
    }

	moduru::Logger::l.setPath(mpc::Paths::logFilePath());

#ifndef __linux__
    auto demoSrc = Paths::demoDataSrcPath();
    auto demoDest = Paths::demoDataDestPath();

	if (!fs::exists(demoDest))
	{
		try
		{
			MLOG("Copying demo data into " + demoDest);
			fs::copy(demoSrc, demoDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		}
		catch (std::exception& e)
		{
            std::string errorMsg = e.what();
			MLOG("An error occurred while copying demo data from " + demoSrc + " to " + demoDest);
		}
	}
	else
	{
		MLOG(demoDest + " already exists, it will not be touched.");
	}
#endif

	hardware = std::make_shared<hardware::Hardware>(*this);
	screens = std::make_shared<Screens>(*this);
	layeredScreen = std::make_shared<lcdgui::LayeredScreen>(*this);
}

void Mpc::init(const int sampleRate, const int inputCount, const int outputCount)
{
	diskController = std::make_unique<mpc::disk::DiskController>(*this);

	sequencer = std::make_shared<mpc::sequencer::Sequencer>(*this);
	MLOG("Sequencer created");

	sampler = std::make_shared<mpc::sampler::Sampler>(*this);
	MLOG("Sampler created");

	mpcMidiInputs = std::vector<mpc::audiomidi::MpcMidiInput*>{ new mpc::audiomidi::MpcMidiInput(*this, 0), new mpc::audiomidi::MpcMidiInput(*this, 1) };

	/*
	* AudioMidiServices requires sequencer to exist.
	*/
	audioMidiServices = std::make_shared<mpc::audiomidi::AudioMidiServices>(*this);
	MLOG("AudioMidiServices created");

	sequencer->init();
	MLOG("Sequencer initialized");

    audioMidiServices->start(sampleRate, inputCount, outputCount);
    MLOG("AudioMidiServices started");

    // This needs to happen before the sampler initializes initMasterPadAssign
    // which we do in Sampler::init()
    mpc::nvram::NvRam::loadVmpcSettings(*this);

    sampler->init();
	MLOG("Sampler initialized");

	eventHandler = std::make_shared<mpc::audiomidi::EventHandler>(*this);
	MLOG("Eeventhandler created");

	controls = std::make_shared<controls::Controls>(*this);

	diskController->initDisks();

	mpc::nvram::NvRam::loadUserScreenValues(*this);

    // We fetch all screens once so they're all cached in Screens,
    // avoiding memory allocations on the audio thread.
	for (auto& screenName : screenNames)
        screens->get<ScreenComponent>(screenName);

    layeredScreen->openScreen("sequencer");

	MLOG("Mpc is ready")
}

std::weak_ptr<controls::Controls> Mpc::getControls()
{
	return controls;
}

std::weak_ptr<hardware::Hardware> Mpc::getHardware()
{
	return hardware;
}

std::weak_ptr<mpc::sequencer::Sequencer> Mpc::getSequencer()
{
    return sequencer;
}

std::weak_ptr<sampler::Sampler> Mpc::getSampler()
{
    return sampler;
}

ctoot::mpc::MpcSoundPlayerChannel* Mpc::getDrum(int i)
{
	auto mms = audioMidiServices->getMms().lock();
	auto channel = mms->getChannel(i).lock().get();
	return dynamic_cast< ctoot::mpc::MpcSoundPlayerChannel*>(channel);
}

std::vector<ctoot::mpc::MpcSoundPlayerChannel*> Mpc::getDrums()
{
    std::vector<ctoot::mpc::MpcSoundPlayerChannel*> drums(4);
	
    for (int i = 0; i < 4; i++)
		drums[i] = getDrum(i);
	
	return drums;
}

ctoot::mpc::MpcBasicSoundPlayerChannel* Mpc::getBasicPlayer()
{
	auto mms = audioMidiServices->getMms().lock();
	auto channel = mms->getChannel(4).lock().get();
	return dynamic_cast< ctoot::mpc::MpcBasicSoundPlayerChannel*>(channel);
}

std::weak_ptr<audiomidi::AudioMidiServices> Mpc::getAudioMidiServices()
{
    return audioMidiServices;
}

std::weak_ptr<audiomidi::EventHandler> Mpc::getEventHandler() {
	return eventHandler;
}

std::weak_ptr<lcdgui::LayeredScreen> Mpc::getLayeredScreen() {
	return layeredScreen;
}

std::weak_ptr<lcdgui::ScreenComponent> Mpc::getActiveControls() {
	return layeredScreen->findScreenComponent().lock();
}

std::shared_ptr<mpc::controls::GlobalReleaseControls> Mpc::getReleaseControls() {
	return controls->getReleaseControls();
}

std::weak_ptr<mpc::disk::AbstractDisk> Mpc::getDisk()
{
	return diskController->getActiveDisk();
}

std::vector<std::shared_ptr<mpc::disk::AbstractDisk>> Mpc::getDisks()
{
    return diskController->getDisks();
}

std::vector<char> Mpc::akaiAsciiChar { ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '}' };
std::vector<std::string> Mpc::akaiAscii { " ", "!", "#", "$", "%", "&", "'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "}" };

ctoot::mpc::MpcMultiMidiSynth* Mpc::getMms()
{
	return audioMidiServices->getMms().lock().get();
}

std::weak_ptr<audiomidi::MpcMidiPorts> Mpc::getMidiPorts()
{
	return audioMidiServices->getMidiPorts();
}

audiomidi::MpcMidiInput* Mpc::getMpcMidiInput(int i)
{
	return mpcMidiInputs[i];
}

void Mpc::setBank(int i)
{
	if (i == bank)
		return;

	if (i < 0 || i > 3)
		return;

	bank = i;

	notifyObservers(std::string("bank"));

	hardware->getLed("pad-bank-a").lock()->light(i == 0);
	hardware->getLed("pad-bank-b").lock()->light(i == 1);
	hardware->getLed("pad-bank-c").lock()->light(i == 2);
	hardware->getLed("pad-bank-d").lock()->light(i == 3);
}

int Mpc::getBank()
{
	return bank;
}

void Mpc::setPadAndNote(int pad, int note)
{
	if (pad < -1 || pad > 63 || note < 34 || note > 98)
		return;

	if (prevPad != pad && pad != -1)
		prevPad = pad;

	this->pad = pad;

	if (note != 34)
		prevNote = note;

	this->note = note;


	notifyObservers(std::string("padandnote"));
}

int Mpc::getNote()
{
	return note;
}

int Mpc::getPad()
{
	return pad;
}

int Mpc::getPrevNote()
{
	return prevNote;
}

int Mpc::getPrevPad()
{
	return prevPad;
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
	mpc::nvram::NvRam::saveUserScreenValues(*this);
	mpc::nvram::NvRam::saveVmpcSettings(*this);

	for (auto& m : mpcMidiInputs)
	{
		if (m != nullptr)
			delete m;
	}

	if (layeredScreen)
		layeredScreen.reset();

	if (audioMidiServices)
		audioMidiServices->destroyServices();
}
