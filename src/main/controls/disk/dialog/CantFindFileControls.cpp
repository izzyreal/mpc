#include <controls/disk/dialog/CantFindFileControls.hpp>

#include <ui/disk/DiskGui.hpp>

using namespace mpc::controls::disk::dialog;
using namespace std;

CantFindFileControls::CantFindFileControls() 
	: AbstractDiskControls()
{
}

void CantFindFileControls::function(int i)
{
	init();
	switch (i) {
	case 1:
		diskGui->setSkipAll(true);
		//ls->getWindowPanel()->removeAll();
		diskGui->setWaitingForUser(false);
		break;
	case 2:
		diskGui->setWaitingForUser(false);
		break;
	}
}
