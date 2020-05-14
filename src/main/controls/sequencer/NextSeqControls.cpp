#include <controls/sequencer/NextSeqControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

NextSeqControls::NextSeqControls()
	: AbstractSequencerControls()
{
}

void NextSeqControls::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0) {
		if (sequencer.lock()->isPlaying()) {
			sequencer.lock()->setNextSq(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + i);
		}
		else {
			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		}
	}
	if (param.compare("nextsq") == 0)
		sequencer.lock()->setNextSq(sequencer.lock()->getNextSq() + i);
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
