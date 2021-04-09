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
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadAProgramScreen.hpp>

#include <file/Directory.hpp>

#include <string>

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::file;

#ifdef _WIN32
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <thirdp/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

using namespace std;

Mpc::Mpc()
{
    vector<string> requiredPaths {
        Paths::appDocumentsPath(),
        Paths::configPath(),
        Paths::storesPath(),
        Paths::defaultStorePath(),
        Paths::recordingsPath()
    };
    
    for (auto& p : requiredPaths) {
        Directory dir(p);
        if (!dir.exists())
            dir.create();
    }
    
	moduru::Logger::l.setPath(mpc::Paths::logFilePath());

#ifndef __linux__
    auto demoSrc = Paths::demoDataSrcPath();
    auto demoDest = Paths::demoDataDestPath();

	if (!Directory(demoDest).exists())
	{
		try
		{
			MLOG("Copying demo data into " + demoDest);
			fs::copy(demoSrc, demoDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		}
		catch (std::exception& e)
		{
			string errorMsg = e.what();
			MLOG("An error occurred while copying demo data from " + demoSrc + " to " + demoDest);
		}
	}
	else
	{
		MLOG(demoDest + " already exists, it will not be touched.");
	}
#endif

	hardware = make_shared<hardware::Hardware>(*this);
	screens = make_shared<Screens>(*this);
	layeredScreen = make_shared<lcdgui::LayeredScreen>(*this);
}

void Mpc::init(const int sampleRate, const int inputCount, const int outputCount)
{
	diskController = make_unique<mpc::disk::DiskController>(*this);
	
	sequencer = make_shared<mpc::sequencer::Sequencer>(*this);
	MLOG("Sequencer created");

	sampler = make_shared<mpc::sampler::Sampler>(*this);
	MLOG("Sampler created");

	mpcMidiInputs = vector<mpc::audiomidi::MpcMidiInput*>{ new mpc::audiomidi::MpcMidiInput(*this, 0), new mpc::audiomidi::MpcMidiInput(*this, 1) };

	/*
	* AudioMidiServices requires sequencer to exist.
	*/
	audioMidiServices = make_shared<mpc::audiomidi::AudioMidiServices>(*this);
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

	eventHandler = make_shared<mpc::audiomidi::EventHandler>(*this);
	MLOG("Eeventhandler created");

	controls = make_shared<controls::Controls>(*this);

	diskController->initDisks();

	mpc::nvram::NvRam::loadUserScreenValues(*this);

    // We fetch all screens once so they're all cached in Screens,
    // avoiding memory allocations on the audio thread.
	for (auto& screenName : screenNames)
        screens->get<ScreenComponent>(screenName);

    layeredScreen->openScreen("sequencer");

	MLOG("Mpc is ready")
}

weak_ptr<controls::Controls> Mpc::getControls()
{
	return controls;
}

weak_ptr<hardware::Hardware> Mpc::getHardware()
{
	return hardware;
}

weak_ptr<mpc::sequencer::Sequencer> Mpc::getSequencer()
{
    return sequencer;
}

weak_ptr<sampler::Sampler> Mpc::getSampler()
{
    return sampler;
}

ctoot::mpc::MpcSoundPlayerChannel* Mpc::getDrum(int i)
{
	auto mms = audioMidiServices->getMms().lock();
	auto channel = mms->getChannel(i).lock().get();
	return dynamic_cast< ctoot::mpc::MpcSoundPlayerChannel*>(channel);
}

vector<ctoot::mpc::MpcSoundPlayerChannel*> Mpc::getDrums()
{
	auto drums = vector<ctoot::mpc::MpcSoundPlayerChannel*>(4);
	for (int i = 0; i < 4; i++) {
		drums[i] = getDrum(i);
	}
	return drums;
}

ctoot::mpc::MpcBasicSoundPlayerChannel* Mpc::getBasicPlayer()
{
	auto mms = audioMidiServices->getMms().lock();
	auto channel = mms->getChannel(4).lock().get();
	return dynamic_cast< ctoot::mpc::MpcBasicSoundPlayerChannel*>(channel);
}

weak_ptr<audiomidi::AudioMidiServices> Mpc::getAudioMidiServices()
{
    return audioMidiServices;
}

weak_ptr<audiomidi::EventHandler> Mpc::getEventHandler() {
	return eventHandler;
}

weak_ptr<lcdgui::LayeredScreen> Mpc::getLayeredScreen() {
	return layeredScreen;
}

weak_ptr<lcdgui::ScreenComponent> Mpc::getActiveControls() {
	return layeredScreen->findScreenComponent().lock();
}

shared_ptr<mpc::controls::GlobalReleaseControls> Mpc::getReleaseControls() {
	return controls->getReleaseControls();
}

weak_ptr<mpc::disk::AbstractDisk> Mpc::getDisk()
{
	return diskController->getDisk();
}

weak_ptr<mpc::disk::Stores> Mpc::getStores()
{
	return diskController->getStores();
}

vector<char> Mpc::akaiAsciiChar { ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '}' };
vector<string> Mpc::akaiAscii { " ", "!", "#", "$", "%", "&", "'", "(", ")", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "}" };

void Mpc::loadSound(bool replace)
{
	if (loadSoundThread.joinable())
		loadSoundThread.join();
	
	auto lDisk = getDisk().lock();
	lDisk->setBusy(true);
	auto soundLoader = mpc::disk::SoundLoader(*this, sampler->getSounds(), replace);
	soundLoader.setPreview(true);
	soundLoader.setPartOfProgram(false);
	bool hasNotBeenLoadedAlready = true;

	auto loadScreen = screens->get<LoadScreen>("load");

	try
	{
		hasNotBeenLoadedAlready = soundLoader.loadSound(loadScreen->getSelectedFile()) == -1;
	}
	catch (const exception& exception)
	{
		sampler->deleteSound(sampler->getSoundCount() - 1);
		MLOG("A problem occurred when trying to load " + loadScreen->getSelectedFileName() + ": " + string(exception.what()));
		lDisk->setBusy(false);
		layeredScreen->openScreen("load");
		return;
	}
	
	if (hasNotBeenLoadedAlready)
	{
		loadSoundThread = thread(&Mpc::runLoadSoundThread, this, soundLoader.getSize());
	}
	else
    {
		sampler->deleteSound(sampler->getSoundCount() - 1);
		lDisk->setBusy(false);
	}
}

void Mpc::loadProgram()
{
	programLoader.reset();

	auto loadScreen = screens->get<LoadScreen>("load");
	auto loadAProgramScreen = screens->get<LoadAProgramScreen>("load-a-program");

	controls->getControls()->init();
	auto activePgm = controls->getControls()->mpcSoundPlayerChannel->getProgram();

	programLoader = make_unique<mpc::disk::ProgramLoader>(*this, loadScreen->getSelectedFile(), loadAProgramScreen->loadReplaceSound ? activePgm : -1);
}

void Mpc::importLoadedProgram()
{
	auto t = sequencer->getActiveSequence().lock()->getTrack(sequencer->getActiveTrackIndex()).lock();
	auto loadAProgramScreen = screens->get<LoadAProgramScreen>("load-a-program");

	if (!loadAProgramScreen->clearProgramWhenLoading)
		getDrum(t->getBus() - 1)->setProgram(sampler->getProgramCount() - 1);
}

ctoot::mpc::MpcMultiMidiSynth* Mpc::getMms()
{
	return audioMidiServices->getMms().lock().get();
}

void Mpc::runLoadSoundThread(mpc::Mpc* mpc, int size)
{
	int sleepTime = size / 400;
	
	if (sleepTime < 300)
	{
		sleepTime = 300;
	}
	
	this_thread::sleep_for(chrono::milliseconds((int)(sleepTime)));
	mpc->getLayeredScreen().lock()->openScreen("load-a-sound");
	mpc->getDisk().lock()->setBusy(false);
}

weak_ptr<audiomidi::MpcMidiPorts> Mpc::getMidiPorts()
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
	{
		return;
	}
	
	if (i < 0 || i > 3)
	{
		return;
	}

	bank = i;

	
	notifyObservers(string("bank"));

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
	{
		return;
	}

	if (prevPad != pad && pad != -1)
	{
		prevPad = pad;
	}

	this->pad = pad;

	if (note != 34)
	{
		prevNote = note;
	}

	this->note = note;

	
	notifyObservers(string("padandnote"));
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

string Mpc::getPreviousSamplerScreenName()
{
	return previousSamplerScreenName;
}

void Mpc::setPreviousSamplerScreenName(string s)
{
	previousSamplerScreenName = s;
}

Mpc::~Mpc() {
	MLOG("Entering Mpc destructor");

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
	{
		audioMidiServices->destroyServices();
		MLOG("AudioMidiServices destroyed.");
	}

	if (loadSoundThread.joinable())
		loadSoundThread.join();
}
