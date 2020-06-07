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

#include <disk/AllLoader.hpp>
#include <disk/ApsLoader.hpp>

#include <disk/MpcFile.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadAProgramScreen.hpp>
#include <string>

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

Mpc::Mpc()
{
	moduru::Logger::l.setPath(mpc::Paths::logFilePath());

	hardware = make_shared<hardware::Hardware>();

	layeredScreen = make_shared<lcdgui::LayeredScreen>();
}

void Mpc::init(const int sampleRate, const int inputCount, const int outputCount)
{
	sequencer = make_shared<mpc::sequencer::Sequencer>();
	MLOG("Sequencer created");

	sampler = make_shared<mpc::sampler::Sampler>();
	MLOG("Sampler created");

	mpcMidiInputs = vector<mpc::audiomidi::MpcMidiInput*>{ new mpc::audiomidi::MpcMidiInput(0), new mpc::audiomidi::MpcMidiInput(1) };

	/*
	* AudioMidiServices requires sampler to exist.
	*/
	audioMidiServices = make_shared<mpc::audiomidi::AudioMidiServices>();
	MLOG("AudioMidiServices created");

	sequencer->init();
	MLOG("Sequencer initialized");

	sampler->init();
	MLOG("Sampler initialized");

	eventHandler = make_shared<mpc::audiomidi::EventHandler>();
	MLOG("Eeventhandler created");

	audioMidiServices->start(sampleRate, inputCount, outputCount);
	MLOG("AudioMidiServices started");

	controls = make_shared<controls::Controls>();

	diskController = make_unique<mpc::disk::DiskController>();
	diskController->initDisks();

	hardware->getSlider().lock()->setValue(mpc::nvram::NvRam::getSlider());
	MLOG("Mpc is ready")
}

weak_ptr<controls::Controls> Mpc::getControls() {
	return controls;
}

weak_ptr<hardware::Hardware> Mpc::getHardware() {
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

controls::BaseControls* Mpc::getActiveControls() {
	return dynamic_pointer_cast<mpc::controls::BaseControls>(layeredScreen->findScreenComponent().lock()).get();
}

controls::GlobalReleaseControls* Mpc::getReleaseControls() {
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
	{
		loadSoundThread.join();
	}
	
	auto lDisk = getDisk().lock();
	lDisk->setBusy(true);
	auto soundLoader = mpc::disk::SoundLoader(sampler->getSounds(), replace);
	soundLoader.setPreview(true);
	soundLoader.setPartOfProgram(false);
	bool hasNotBeenLoadedAlready = true;

	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

	try
	{
		hasNotBeenLoadedAlready = soundLoader.loadSound(loadScreen->getSelectedFile()) == -1;
	}
	catch (const invalid_argument& exception)
	{
		MLOG("A problem occurred when trying to load " + loadScreen->getSelectedFileName() + ": " + string(exception.what()));
		lDisk->setBusy(false);
		layeredScreen->removePopup();
		return;
	}
	
	if (hasNotBeenLoadedAlready)
	{
		loadSoundThread = thread(&Mpc::static_loadSound, soundLoader.getSize());
	}
	else {
		sampler->deleteSample(sampler->getSoundCount() - 1);
		lDisk->setBusy(false);
	}
}

void Mpc::loadProgram()
{
	programLoader.reset();

	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
	auto loadAProgramScreen = dynamic_pointer_cast<LoadAProgramScreen>(Screens::getScreenComponent("load-a-program"));

	programLoader = make_unique<mpc::disk::ProgramLoader>(loadScreen->getSelectedFile(), loadAProgramScreen->loadReplaceSound);
}

void Mpc::importLoadedProgram()
{
	auto t = sequencer->getActiveSequence().lock()->getTrack(sequencer->getActiveTrackIndex()).lock();
	auto loadAProgramScreen = dynamic_pointer_cast<LoadAProgramScreen>(Screens::getScreenComponent("load-a-program"));

	if (loadAProgramScreen->clearProgramWhenLoading)
	{
		auto pgm = getDrum(t->getBusNumber() - 1)->getProgram();
		sampler->replaceProgram(programLoader->get(), pgm);
	}
	else {
		getDrum(t->getBusNumber() - 1)->setProgram(sampler->getProgramCount() - 1);
	}
}

ctoot::mpc::MpcMultiMidiSynth* Mpc::getMms()
{
	return audioMidiServices->getMms().lock().get();
}

void Mpc::static_loadSound(int size)
{
	Mpc::instance().runLoadSoundThread(size);
}

void Mpc::runLoadSoundThread(int size) {
	int sleepTime = size / 400;
	
	if (sleepTime < 300)
	{
		sleepTime = 300;
	}
	
	this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 0.1)));
	layeredScreen->removePopup();
	layeredScreen->openScreen("load-a-sound");
	getDisk().lock()->setBusy(false);
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

	setChanged();
	notifyObservers(string("bank"));

	hardware->getLed("padbanka").lock()->light(i == 0);
	hardware->getLed("padbankb").lock()->light(i == 1);
	hardware->getLed("padbankc").lock()->light(i == 2);
	hardware->getLed("padbankd").lock()->light(i == 3);
}

int Mpc::getBank()
{
	return bank;
}

Mpc::~Mpc() {
	MLOG("Mpc destructor.");

	mpc::nvram::NvRam::saveUserDefaults();

	for (auto& m : mpcMidiInputs) {
		if (m != nullptr) {
			delete m;
		}
	}

    if (layeredScreen) layeredScreen.reset();
	if (audioMidiServices) audioMidiServices->destroyServices();
	MLOG("audio midi services destroyed.");
	if (loadSoundThread.joinable()) loadSoundThread.join();
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

	setChanged();
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
