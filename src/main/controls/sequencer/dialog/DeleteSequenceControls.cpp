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
	
	if (param.compare("sq") == 0) {
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
	}
}

void DeleteSequenceControls::function(int i)
{
	init();
	
	switch (i) {
	case 2:
		ls.lock()->openScreen("deleteallsequences");
		break;
	case 3:
		ls.lock()->openScreen("sequence");
		break;
	case 4:
		sequencer.lock()->purgeSequence(sequencer.lock()->getActiveSequenceIndex());
		ls.lock()->openScreen("sequencer");
		break;
	}
}
