#include <file/aps/ApsSaver.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/aps/ApsParser.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::file::aps;
using namespace std;

ApsSaver::ApsSaver(mpc::Mpc* mpc, string apsFileName)
{
	this->mpc = mpc;
	this->apsFileName = apsFileName;
	auto lDisk = mpc->getDisk().lock();
	lDisk->initFiles();
	if (lDisk->checkExists(apsFileName)) {
		mpc->getLayeredScreen().lock()->openScreen("filealreadyexists");
	}
	else {
		saveAps();
	}
}


void ApsSaver::saveAps()
{
	auto lDisk = mpc->getDisk().lock();
	lDisk->setBusy(true);
    auto file = lDisk->newFile(apsFileName);
    ApsParser apsParser(mpc, apsFileName.substr(0, apsFileName.find(".")));
    auto bytes = apsParser.getBytes();
    file->setFileData(&bytes);
	auto const saveWith = mpc->getUis().lock()->getDiskGui()->getPgmSave();
	if (saveWith != 0) {
		soundSaver = std::make_unique<mpc::disk::SoundSaver>(mpc, mpc->getSampler().lock()->getSounds(), saveWith == 1 ? false : true);
	}
	else {
		lDisk->setBusy(false);
		mpc->getLayeredScreen().lock()->openScreen("save");
	}
}
