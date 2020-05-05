#include "ApsSaver.hpp"

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/aps/ApsParser.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::disk;
using namespace mpc::file::aps; 
using namespace std;

ApsSaver::ApsSaver(string apsFileName)
{
	
	this->apsFileName = apsFileName;
	auto lDisk = Mpc::instance().getDisk().lock();
	lDisk->initFiles();
	if (lDisk->checkExists(apsFileName)) {
		Mpc::instance().getLayeredScreen().lock()->openScreen("filealreadyexists");
	}
	else {
		saveAps();
	}
}


void ApsSaver::saveAps()
{
	auto lDisk = Mpc::instance().getDisk().lock();
	lDisk->setBusy(true);
    auto file = lDisk->newFile(apsFileName);
	ApsParser apsParser(apsFileName.substr(0, apsFileName.find(".")));
    auto bytes = apsParser.getBytes();
    file->setFileData(&bytes);
	auto const saveWith = Mpc::instance().getUis().lock()->getDiskGui()->getPgmSave();
	if (saveWith != 0) {
		soundSaver = make_unique<mpc::disk::SoundSaver>(Mpc::instance().getSampler().lock()->getSounds(), saveWith == 1 ? false : true);
	}
	else {
		lDisk->setBusy(false);
		Mpc::instance().getLayeredScreen().lock()->openScreen("save");
	}
}
