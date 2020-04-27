#include <nvram/NvRam.hpp>

#include <file/all/Defaults.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <StartUp.hpp>
#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <ui/UserDefaults.hpp>
#include <nvram/DefaultsParser.hpp>
#include <nvram/KnobPositions.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::nvram;
using namespace moduru::file;
using namespace std;

NvRam::NvRam()
{
}

shared_ptr<mpc::ui::UserDefaults> NvRam::load()
{
	string path = mpc::StartUp::resPath + "nvram.vmp";
	auto file = File(path, nullptr);

	auto ud = make_shared<mpc::ui::UserDefaults>();
	
	if (!file.exists()) {
		file.create();
		auto stream = FileUtil::ofstreamw(path, ios::binary | ios::out);
		auto dp = DefaultsParser(ud);
		auto bytes = dp.getBytes();
		stream.write(&bytes[0], bytes.size());
		stream.close();
	}
	else {
		auto defaults = DefaultsParser::AllDefaultsFromFile(file);
		
		ud->setLastBar(defaults.getBarCount() - 1);
		ud->setBus((defaults.getBusses())[0]);
		
		for (int i = 0; i < 33; i++) {
			ud->setDeviceName(i, defaults.getDefaultDevNames()[i]);
		}
		ud->setSequenceName(defaults.getDefaultSeqName());
		auto defTrackNames = defaults.getDefaultTrackNames();
		for (int i = 0; i < 64; i++) {
			ud->setTrackName(i, defTrackNames[i]);
		}
		ud->setDeviceNumber(defaults.getDevices()[0]);
		ud->setTimeSig(defaults.getTimeSigNum(), defaults.getTimeSigDen());
		ud->setPgm(defaults.getPgms()[0]);
		ud->setTempo(defaults.getTempo() / 10.0);
		ud->setVelo(defaults.getTrVelos()[0]);
	}
    return ud;
}

void NvRam::saveUserDefaults()
{
	auto dp = DefaultsParser(mpc::StartUp::getUserDefaults());
	
	string fileName = mpc::StartUp::resPath + "nvram.vmp";
	
	auto file = moduru::file::File(fileName, nullptr);
	
	if (!file.exists()) {
		file.create();
	}

	auto stream = FileUtil::ofstreamw(fileName, ios::binary | ios::out);
	auto bytes = dp.getBytes();
	stream.write(&bytes[0], bytes.size());
	stream.close();
}

void NvRam::saveKnobPositions(mpc::Mpc* mpc)
{
    auto ams = mpc->getAudioMidiServices().lock();
    auto hw = mpc->getHardware().lock();
    
	std::shared_ptr<mpc::hardware::Slider> slider;
    
	// Can we remove this check?
	if (hw) {
		slider = hw->getSlider().lock();
	}

    if (ams && hw && slider) {
        
		auto file = moduru::file::File(mpc::StartUp::resPath + "knobpositions.vmp", nullptr);
		
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
