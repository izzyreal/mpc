#include "SaveApsFileControls.hpp"

#include <Util.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

SaveApsFileControls::SaveApsFileControls()
	: AbstractDiskControls()
{
}

void SaveApsFileControls::turnWheel(int i)
{
	init();
	if (param.compare("file") == 0) {
		nameGui->setParameterName("saveapsname");
		ls.lock()->openScreen("name");
	}
	else if (param.compare("save") == 0) {
		diskGui->setSave(diskGui->getPgmSave() + i);
	}
	else if (param.compare("replacesamesounds") == 0) {
		diskGui->setSaveReplaceSameSounds(i > 0);
	}
}

void SaveApsFileControls::function(int i)
{
	init();
	string apsFileName;
	switch (i) {
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		apsFileName = mpc::Util::getFileName(nameGui->getName()) + ".APS";
		apsSaver = make_unique<mpc::file::aps::ApsSaver>(apsFileName);
		break;
	}
}
