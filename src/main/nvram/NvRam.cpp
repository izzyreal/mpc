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
#include <io/FileOutputStream.hpp>

using namespace mpc::nvram;
using namespace std;

NvRam::NvRam()
{
}

mpc::ui::UserDefaults* NvRam::load()
{
	string fileName = mpc::StartUp::resPath + "nvram.vmp";
	auto file = new moduru::file::File(fileName, nullptr);
    auto ud = new mpc::ui::UserDefaults();
	if (!file->exists()) {
		file->create();
		auto fos = new moduru::io::FileOutputStream(file);
		auto dp = new DefaultsParser(ud);
		fos->write(dp->getBytes());
		fos->close();
	}
	else {
		auto dp = new DefaultsParser(file);
		auto defaults = dp->getDefaults();
		ud->setLastBar(defaults->getBarCount() - 1);
		ud->setBus((defaults->getBusses())[0]);
		for (int i = 0; i < 33; i++) {
			ud->setDeviceName(i, defaults->getDefaultDevNames()[i]);
		}
		ud->setSequenceName(defaults->getDefaultSeqName());
		auto defTrackNames = defaults->getDefaultTrackNames();
		for (int i = 0; i < 64; i++) {
			ud->setTrackName(i, defTrackNames[i]);
		}
		ud->setDeviceNumber(defaults->getDevices()[0]);
		ud->setTimeSig(defaults->getTimeSigNum(), defaults->getTimeSigDen());
		ud->setPgm(defaults->getPgms()[0]);
		ud->setTempo(defaults->getTempo() / 10.0);
		ud->setVelo(defaults->getTrVelos()[0]);
	}
    return ud;
}

void NvRam::saveUserDefaults()
{
	auto dp = new DefaultsParser(mpc::StartUp::getUserDefaults().lock().get());
	string fileName = mpc::StartUp::resPath + "nvram.vmp";
	auto file = new moduru::file::File(fileName, nullptr);
	if (!file->exists()) {
		file->create();
	}
	auto fos = new moduru::io::FileOutputStream(file);
	fos->write(dp->getBytes());
	fos->close();
}

void NvRam::saveKnobPositions(mpc::Mpc* mpc)
{
    auto ams = mpc->getAudioMidiServices().lock();
    auto hw = mpc->getHardware().lock();
    std::shared_ptr<mpc::hardware::Slider> slider;
    if (hw) slider = hw->getSlider().lock();
    if (ams && hw && slider) {
        auto file = new moduru::file::File(mpc::StartUp::resPath + "knobpositions.vmp", nullptr);
        if (!file->exists())
            file->create();

        auto fos = new moduru::io::FileOutputStream(file);
        char recordb = ams->getRecordLevel();
        char volumeb = ams->getMasterLevel();
		if (volumeb == 0) MLOG("Volume is 0!");
        char sliderb = static_cast< int8_t >(slider->getValue());
        auto bytes = vector<char>{ recordb, volumeb, sliderb };
        fos->write(bytes);
        fos->close();
    }
}

int NvRam::getMasterLevel()
{
    return getKnobPositions()->masterLevel;
}

int NvRam::getRecordLevel()
{
    
    return getKnobPositions()->recordLevel;
}

int NvRam::getSlider()
{
    
    return getKnobPositions()->slider;
}

KnobPositions* NvRam::getKnobPositions()
{
    
    return new KnobPositions();
}
