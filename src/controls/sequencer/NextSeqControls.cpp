#include <controls/sequencer/NextSeqControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

NextSeqControls::NextSeqControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void NextSeqControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("sq") == 0) {
		if (lSequencer->isPlaying()) {
			lSequencer->setNextSq(lSequencer->getCurrentlyPlayingSequenceIndex() + i);
		}
		else {
			lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex() + i);
		}
	}
	if (param.compare("nextsq") == 0)
		lSequencer->setNextSq(lSequencer->getNextSq() + i);
}

void NextSeqControls::function(int i)
{
	init();
	switch (i) {
	case 5:
		ls.lock()->openScreen("nextseqpad");
		break;
	}
}
