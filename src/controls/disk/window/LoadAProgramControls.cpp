#include <controls/disk/window/LoadAProgramControls.hpp>

#include <Mpc.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

LoadAProgramControls::LoadAProgramControls(mpc::Mpc* mpc) 
	: AbstractDiskControls(mpc)
{
}

void LoadAProgramControls::turnWheel(int i)
{
	init();
	if (param.compare("loadreplacesound") == 0) {
		diskGui->setLoadReplaceSound(i > 0);
	}
}

void LoadAProgramControls::function(int i)
{
	init();
	switch (i) {
	case 2:
		diskGui->setClearProgramWhenLoading(true);
		mpc->loadProgram();
		break;
	case 3:
		ls.lock()->openScreen("load");
		break;
	case 4:
		diskGui->setClearProgramWhenLoading(false);
		mpc->loadProgram();
		break;
	}
}
