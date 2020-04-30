#include <controls/disk/window/MPC2000XLAllFileControls.hpp>

#include <disk/AllLoader.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

MPC2000XLAllFileControls::MPC2000XLAllFileControls() 
	: AbstractDiskControls()
{
}

void MPC2000XLAllFileControls::function(int i)
{
	init();
	mpc::disk::AllLoader* allLoader = nullptr;
	switch (i) {
	case 2:
	{
		auto sequencesOnly = true;
		mpc::disk::AllLoader allLoader(diskGui->getSelectedFile(), sequencesOnly);
		diskGui->setSequencesFromAllFile(allLoader.getSequences());
		diskGui->setFileLoad(0);
		ls.lock()->openScreen("loadasequencefromall");
		break;
	}
	case 3:
		ls.lock()->openScreen("load");
		break;
	case 4:
	{
		auto sequencesOnly = false;
		mpc::disk::AllLoader allLoader(diskGui->getSelectedFile(), sequencesOnly);
		ls.lock()->openScreen("sequencer");
		break;
	}
	}

	if (allLoader != nullptr) {
		delete allLoader;
	}
}
