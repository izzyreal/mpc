#include "NvRam.hpp"

#include <nvram/DefaultsParser.hpp>
#include <nvram/KnobPositions.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

#include <audiomidi/AudioMidiServices.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>

#include <file/all/Defaults.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::nvram;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;
using namespace std;

void NvRam::loadUserScreenValues(mpc::Mpc& mpc)
{
	string path = mpc::Paths::resPath() + "nvram.vmp";
	auto file = File(path, nullptr);

	if (!file.exists())
		return;

	auto defaults = DefaultsParser::AllDefaultsFromFile(mpc, file);
	auto userScreen = mpc.screens->get<UserScreen>("user");

	userScreen->lastBar = defaults.getBarCount() - 1;
	userScreen->bus = defaults.getBusses()[0];

	for (int i = 0; i < 33; i++)
		userScreen->setDeviceName(i, defaults.getDefaultDevNames()[i]);
	
	userScreen->setSequenceName(defaults.getDefaultSeqName());
	auto defTrackNames = defaults.getDefaultTrackNames();

	for (int i = 0; i < 64; i++)
		userScreen->setTrackName(i, defTrackNames[i]);

	userScreen->setDeviceNumber(defaults.getDevices()[0]);
	userScreen->setTimeSig(defaults.getTimeSigNum(), defaults.getTimeSigDen());
	userScreen->setPgm(defaults.getPgms()[0]);
	userScreen->setTempo(defaults.getTempo() / 10.0);
	userScreen->setVelo(defaults.getTrVelos()[0]);
}

void NvRam::saveUserScreenValues(mpc::Mpc& mpc)
{
	DefaultsParser dp(mpc);
	
	string fileName = mpc::Paths::resPath() + "nvram.vmp";
	
	File file(fileName, nullptr);
	
	if (!file.exists())
		file.create();

	auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);
	auto bytes = dp.getBytes();
	stream.write(&bytes[0], bytes.size());
	stream.close();
}

void NvRam::saveKnobPositions(mpc::Mpc& mpc)
{
    auto ams = mpc.getAudioMidiServices().lock();
    auto hw = mpc.getHardware().lock();
    
	shared_ptr<mpc::hardware::Slider> slider;
    
	// Can we remove this check?
	if (hw)
		slider = hw->getSlider().lock();

    if (ams && hw && slider)
	{    
		File file(mpc::Paths::resPath() + "knobpositions.vmp", nullptr);
		
		if (!file.exists())
			file.create();

		char recordb = ams->getRecordLevel();
        char volumeb = ams->getMasterLevel();
		
		char sliderb = static_cast<int8_t>(slider->getValue());
        auto bytes = vector<char>{ recordb, volumeb, sliderb };
        
		auto stream = FileUtil::ofstreamw(file.getPath(), ios::binary | ios::out);
		stream.write(&bytes[0], bytes.size());
    }
}

int NvRam::getMasterLevel()
{
    return KnobPositions().masterLevel;
}

int NvRam::getRecordLevel()
{
    return KnobPositions().recordLevel;
}

int NvRam::getSlider()
{
    return KnobPositions().slider;
}

void NvRam::saveVmpcSettings(mpc::Mpc& mpc)
{
	auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
	string fileName = mpc::Paths::resPath() + "vmpc-specific.ini";

	File file(fileName, nullptr);

	if (!file.exists())
		file.create();

	auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);
	
    vector<char> bytes{
        (char) (vmpcSettingsScreen->initialPadMapping),
        (char) (vmpcSettingsScreen->_16LevelsEraseMode)
    };
    
	stream.write(&bytes[0], bytes.size());
	stream.close();
}

void NvRam::loadVmpcSettings(mpc::Mpc& mpc)
{
	string path = mpc::Paths::resPath() + "vmpc-specific.ini";
	File file(path, nullptr);

	if (!file.exists() || file.getLength() != 2)
		return;

	auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
	
	vector<char> bytes(2);
	file.getData(&bytes);

	vmpcSettingsScreen->initialPadMapping = bytes[0];
    vmpcSettingsScreen->_16LevelsEraseMode = bytes[1];
}
