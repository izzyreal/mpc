#include "ApsSaver.hpp"

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/aps/ApsParser.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>

using namespace mpc::disk;
using namespace mpc::file::aps; 
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ApsSaver::ApsSaver(string apsFileName)
{
	
	this->apsFileName = apsFileName;
	auto lDisk = Mpc::instance().getDisk().lock();

	if (lDisk->checkExists(apsFileName)) {
		Mpc::instance().getLayeredScreen().lock()->openScreen("filealreadyexists");
	}
	else {
		saveAps();
	}
}


void ApsSaver::saveAps()
{
	auto disk = Mpc::instance().getDisk().lock();
	disk->setBusy(true);
    auto file = disk->newFile(apsFileName);
	ApsParser apsParser(apsFileName.substr(0, apsFileName.find(".")));
    auto bytes = apsParser.getBytes();
    file->setFileData(&bytes);

	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(Screens::getScreenComponent("save-a-program"));
	
	if (saveAProgramScreen->save != 0)
	{
		soundSaver = make_unique<mpc::disk::SoundSaver>(Mpc::instance().getSampler().lock()->getSounds(), saveAProgramScreen->save == 1 ? false : true);
	}
	else
	{
		disk->setBusy(false);
		Mpc::instance().getLayeredScreen().lock()->openScreen("save");
	}
}
