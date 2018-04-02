#include <controls/disk/window/LoadASequenceFromAllControls.hpp>

#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

LoadASequenceFromAllControls::LoadASequenceFromAllControls(mpc::Mpc* mpc) 
	: AbstractDiskControls(mpc)
{
}

void LoadASequenceFromAllControls::turnWheel(int i)
{
	init();
	if (param.compare("file") == 0) {
		diskGui->setFileLoad(diskGui->getFileLoad() + i);
	}
	else if (param.compare("loadinto") == 0) {
		diskWindowGui->setLoadInto(diskWindowGui->getLoadInto() + i);
	}
}

void LoadASequenceFromAllControls::function(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	switch (i) {
	case 2:
		break;
	case 3:
		ls.lock()->openScreen("mpc2000xlallfile");
		break;
	case 4:
		lSequencer->setSequence(lSequencer->getActiveSequenceIndex(), diskGui->getSequencesFromAllFile()->at(diskGui->getFileLoad()));
		ls.lock()->openScreen("sequencer");
		break;
	}
}
