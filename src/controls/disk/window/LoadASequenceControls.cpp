#include <controls/disk/window/LoadASequenceControls.hpp>

#include <ui/disk/window/DiskWindowGui.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

LoadASequenceControls::LoadASequenceControls(mpc::Mpc* mpc) 
	: AbstractDiskControls(mpc)
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
	auto lSequencer = sequencer.lock();
	switch (i) {
	case 3:
		ls.lock()->openScreen("load");
		lSequencer->clearPlaceHolder();
		break;
	case 4:
		lSequencer->movePlaceHolderTo(diskWindowGui->getLoadInto());
		ls.lock()->openScreen("sequencer");
		lSequencer->setActiveSequenceIndex(diskWindowGui->getLoadInto());
		break;
	}
}
