#include "SaveAProgramControls.hpp"

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Program.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

SaveAProgramControls::SaveAProgramControls() 
	: AbstractDiskControls()
{
}

void SaveAProgramControls::turnWheel(int i)
{
	init();
	if (param.compare("save") == 0) {
		diskGui->setSave(diskGui->getPgmSave() + i);
	}
	else if (param.compare("replacesamesounds") == 0) {
		diskGui->setSaveReplaceSameSounds(i > 0);
	}
}

void SaveAProgramControls::function(int i)
{
	init();
	string fileName;
	switch (i) {
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		fileName = mpc::Util::getFileName(nameGui->getName()) + ".PGM";
		auto lDisk = disk.lock();
		auto lProgram = program.lock();
		if (lDisk->checkExists(fileName)) {
			nameGui->setName(lProgram->getName());
			ls.lock()->openScreen("filealreadyexists");
			break;
		}
		lDisk->writeProgram(lProgram.get(), fileName);
		break;
	}
}
