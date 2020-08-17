#include "ApsSaver.hpp"

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/aps/ApsParser.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>

using namespace mpc::disk;
using namespace mpc::file::aps; 
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ApsSaver::ApsSaver(mpc::Mpc& mpc, string apsFileName)
	: mpc(mpc)
{
	this->apsFileName = apsFileName;
	auto disk = mpc.getDisk().lock();

	if (disk->checkExists(apsFileName))
		mpc.getLayeredScreen().lock()->openScreen("file-already-exists");
	else
		saveAps();
}

void ApsSaver::saveAps()
{
	auto disk = mpc.getDisk().lock();
	disk->setBusy(true);
    auto file = disk->newFile(apsFileName);
	ApsParser apsParser(mpc, apsFileName.substr(0, apsFileName.find(".")));
    auto bytes = apsParser.getBytes();
    file->setFileData(&bytes);

	auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(mpc.screens->getScreenComponent("save-a-program"));
	
	if (saveAProgramScreen->save != 0)
	{
		soundSaver = make_unique<mpc::disk::SoundSaver>(mpc, mpc.getSampler().lock()->getSounds(), saveAProgramScreen->save == 2);
	}
	else
	{
		disk->setBusy(false);
		mpc.getLayeredScreen().lock()->openScreen("save");
	}
}
