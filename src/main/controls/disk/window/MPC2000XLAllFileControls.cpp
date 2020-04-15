#include <controls/disk/window/MPC2000XLAllFileControls.hpp>

#include <disk/AllLoader.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

MPC2000XLAllFileControls::MPC2000XLAllFileControls(mpc::Mpc* mpc) 
	: AbstractDiskControls(mpc)
{
}

void MPC2000XLAllFileControls::function(int i)
{
	init();
	mpc::disk::AllLoader* allLoader = nullptr;
	switch (i) {
	case 2:
		allLoader = new mpc::disk::AllLoader(diskGui->getSelectedFile());
		diskGui->setSequencesFromAllFile(allLoader->getSequences());
		diskGui->setFileLoad(0);
		ls.lock()->openScreen("loadasequencefromall");
		break;
	case 3:
		ls.lock()->openScreen("load");
		break;
	case 4:
		allLoader = new mpc::disk::AllLoader(mpc, diskGui->getSelectedFile());
		ls.lock()->openScreen("sequencer");
		break;
	}

	if (allLoader != nullptr) {
		delete allLoader;
	}
}
