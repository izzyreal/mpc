#include <controls/sequencer/dialog/DeleteSequenceControls.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

DeleteSequenceControls::DeleteSequenceControls()
	: AbstractSequencerControls()
{
}

void DeleteSequenceControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("sq") == 0) {
		lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex() + i);
	}
}

void DeleteSequenceControls::function(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	switch (i) {
	case 2:
		ls.lock()->openScreen("deleteallsequences");
		break;
	case 3:
		ls.lock()->openScreen("sequence");
		break;
	case 4:
		lSequencer->purgeSequence(lSequencer->getActiveSequenceIndex());
		ls.lock()->openScreen("sequencer");
		break;
	}
}
