#include "SaveASequenceControls.hpp"

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

SaveASequenceControls::SaveASequenceControls(mpc::Mpc* mpc)
	: AbstractDiskControls(mpc)
{
}

void SaveASequenceControls::turnWheel(int i)
{
	init();
	if (param.compare("saveas") == 0) {
		diskGui->setSaveSequenceAs(diskGui->getSaveSequenceAs() + i);
	}
	else if (param.compare("file") == 0) {
		nameGui->setParameterName("savesequencename");
		ls.lock()->openScreen("name");
	}
}

void SaveASequenceControls::function(int i)
{
	init();
	auto lDisk = disk.lock();
	string fileName;
	shared_ptr<mpc::sequencer::Sequence> seq;
	switch (i) {
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		fileName = mpc::Util::getFileName(nameGui->getName()) + ".MID";
		if (lDisk->checkExists(fileName)) {
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		seq = sequencer.lock()->getActiveSequence().lock();
		lDisk->writeSequence(seq.get(), fileName);
		ls.lock()->openScreen("save");
		break;
	}
}
