#include "SaveAllFileControls.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

SaveAllFileControls::SaveAllFileControls()
	: AbstractDiskControls()
{
}

void SaveAllFileControls::turnWheel(int i)
{
	init();
	if (param.compare("file") == 0) {
		nameGui->setParameterName("saveallfile");
		ls.lock()->openScreen("name");
	}
}

void SaveAllFileControls::function(int i)
{
	init();
	string allName;
	auto lDisk = disk.lock();
	mpc::disk::MpcFile* f;
	switch (i) {
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		allName = mpc::Util::getFileName(nameGui->getName());
		auto existStr = allName + ".ALL";
		if (lDisk->checkExists(existStr)) {
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		allParser = make_unique<mpc::file::all::AllParser>(mpc::Util::getFileName(nameGui->getName()));
		f = lDisk->newFile(allName + ".ALL");
        auto bytes = allParser->getBytes();
		f->setFileData(&bytes);
		lDisk->flush();
		lDisk->initFiles();
		ls.lock()->openScreen("save");
		break;
	}
}
