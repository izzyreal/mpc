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
#include <audiomidi/MpcMidiInput.hpp>
#include <audiomidi/MidiDeviceDetector.hpp>

#include <sampler/Sampler.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcBasicSoundPlayerChannel.hpp>
#include <mpc/MpcMultiMidiSynth.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <midi/core/MidiTransport.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/Led.hpp>

#include <lcdgui/Screens.hpp>

#include "file/FileUtil.hpp"

#include <string>

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpc);

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
        Paths::recordingsPath(),
        Paths::midiControlPresetsPath()
    };

    for (auto& p : requiredPaths) {
        if (!fs::exists(p))
        {
            fs::create_directories(p);

            if (p == Paths::midiControlPresetsPath())
            {
                auto fs = cmrc::mpc::get_filesystem();

                for (auto&& entry : fs.iterate_directory("midicontrolpresets"))
                {
                    if (entry.is_directory()) continue;
                    auto file = fs.open("midicontrolpresets/" + entry.filename());
                    char* data = (char*) std::string_view(file.begin(), file.end() - file.begin()).data();
                    auto path = fs::path(p + entry.filename());
                    auto presetStream = moduru::file::FileUtil::fopenw(path, "wb");
                    std::fwrite(data, sizeof data[0], file.size(), presetStream);
                    std::fclose(presetStream);
                }
            }
        }
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
}

void Mpc::init(const int inputCount, const int outputCount)
{
	diskController = std::make_unique<mpc::disk::DiskController>(*this);

	sequencer = std::make_shared<mpc::sequencer::Sequencer>(*this);
	MLOG("Sequencer created");

	sampler = std::make_shared<mpc::sampler::Sampler>(*this);
	MLOG("Sampler created");

	mpcMidiInputs = std::vector<mpc::audiomidi::MpcMidiInput*>{ new mpc::audiomidi::MpcMidiInput(*this, 0), new mpc::audiomidi::MpcMidiInput(*this, 1) };

    screens = std::make_shared<Screens>(*this);

    /*
    * AudioMidiServices requires sequencer to exist.
    */
	audioMidiServices = std::make_shared<mpc::audiomidi::AudioMidiServices>(*this);
	MLOG("AudioMidiServices created");

    layeredScreen = std::make_shared<lcdgui::LayeredScreen>(*this);

    sequencer->init();
	MLOG("Sequencer initialized");

    audioMidiServices->start(inputCount, outputCount);
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

    midiDeviceDetector = std::make_unique<MidiDeviceDetector>();
    midiDeviceDetector->start(*this);

	MLOG("Mpc is ready");
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

ctoot::mpc::MpcSoundPlayerChannel* Mpc::getDrum(int i)
{
	auto mms = audioMidiServices->getMms();
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
	auto mms = audioMidiServices->getMms();
	auto channel = mms->getChannel(4).lock().get();
	return dynamic_cast< ctoot::mpc::MpcBasicSoundPlayerChannel*>(channel);
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

ctoot::mpc::MpcMultiMidiSynth* Mpc::getMms()
{
	return audioMidiServices->getMms().get();
}

std::shared_ptr<audiomidi::MpcMidiOutput> Mpc::getMidiOutput()
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
    midiDeviceDetector->stop();
    sampler->stopAllVoices(0);
    sequencer->stop();
    mpc::nvram::MidiControlPersistence::saveCurrentState(*this);
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

void Mpc::setPad(unsigned char padIndexWithBank)
{
    if (padIndexWithBank > 63)
    {
        return;
    }

    pad = padIndexWithBank;

    notifyObservers(std::string("pad"));
}
