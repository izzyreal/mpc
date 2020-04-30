#include <controls/sequencer/window/LoopBarsControls.hpp>

#include <sequencer/Sequence.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

LoopBarsControls::LoopBarsControls()
	: AbstractSequencerControls()
{
}

void LoopBarsControls::turnWheel(int i)
{
	init();
	auto seq = sequence.lock();
	if (param.compare("firstbar") == 0) {
		seq->setFirstLoopBar(seq->getFirstLoopBar() + i);
	}
	else if (param.compare("lastbar") == 0) {
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
	}
	else if (param.compare("numberofbars") == 0 && i < 0) {
		if (seq->isLastLoopBarEnd()) {
			seq->setLastLoopBar(seq->getLastLoopBar() - 1);
			seq->setLastLoopBar(seq->getLastLoopBar() - 1);
			return;
		}
		if (seq->getLastLoopBar() > seq->getFirstLoopBar()) {
			seq->setLastLoopBar(seq->getLastLoopBar() - 1);
		}
	}
	else if (param.compare("numberofbars") == 0) {
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
	}
}
