#include "NvRam.hpp"

#include <nvram/DefaultsParser.hpp>
#include <nvram/KnobPositions.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/screens/UserScreen.hpp>

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

void NvRam::loadUserScreenValues()
{
	string path = mpc::Paths::resPath() + "nvram.vmp";
	auto file = File(path, nullptr);

	if (!file.exists())
	{
		return;
	}

	auto defaults = DefaultsParser::AllDefaultsFromFile(file);
	auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));

	userScreen->lastBar = defaults.getBarCount() - 1;
	userScreen->bus = defaults.getBusses()[0];

	for (int i = 0; i < 33; i++)
	{
		userScreen->setDeviceName(i, defaults.getDefaultDevNames()[i]);
	}
	
	userScreen->setSequenceName(defaults.getDefaultSeqName());
	auto defTrackNames = defaults.getDefaultTrackNames();

	for (int i = 0; i < 64; i++)
	{
		userScreen->setTrackName(i, defTrackNames[i]);
	}

	userScreen->setDeviceNumber(defaults.getDevices()[0]);
	userScreen->setTimeSig(defaults.getTimeSigNum(), defaults.getTimeSigDen());
	userScreen->setPgm(defaults.getPgms()[0]);
	userScreen->setTempo(defaults.getTempo() / 10.0);
	userScreen->setVelo(defaults.getTrVelos()[0]);
}

void NvRam::saveUserScreenValues()
{
	DefaultsParser dp;
	
	string fileName = mpc::Paths::resPath() + "nvram.vmp";
	
	auto file = moduru::file::File(fileName, nullptr);
	
	if (!file.exists())
	{
		file.create();
	}

	auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);
	auto bytes = dp.getBytes();
	stream.write(&bytes[0], bytes.size());
	stream.close();
}

void NvRam::saveKnobPositions()
{
    auto ams = Mpc::instance().getAudioMidiServices().lock();
    auto hw = Mpc::instance().getHardware().lock();
    
	std::shared_ptr<mpc::hardware::Slider> slider;
    
	// Can we remove this check?
	if (hw) {
		slider = hw->getSlider().lock();
	}

    if (ams && hw && slider) {
        
		auto file = moduru::file::File(mpc::Paths::resPath() + "knobpositions.vmp", nullptr);
		
		if (!file.exists()) {
			file.create();
		}

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
