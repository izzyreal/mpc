#include <controls/disk/window/LoadASequenceControls.hpp>

#include <ui/disk/window/DiskWindowGui.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

LoadASequenceControls::LoadASequenceControls() 
	: AbstractDiskControls()
{
}

void LoadASequenceControls::turnWheel(int i)
{
	init();
	if (param.compare("loadinto") == 0) {
		diskWindowGui->setLoadInto(diskWindowGui->getLoadInto() + i);
	}
}

void LoadASequenceControls::function(int i)
{
	init();
	
	switch (i) {
	case 3:
		ls.lock()->openScreen("load");
		sequencer.lock()->clearPlaceHolder();
		break;
	case 4:
		sequencer.lock()->movePlaceHolderTo(diskWindowGui->getLoadInto());
		ls.lock()->openScreen("sequencer");
		sequencer.lock()->setActiveSequenceIndex(diskWindowGui->getLoadInto());
		break;
	}
}
