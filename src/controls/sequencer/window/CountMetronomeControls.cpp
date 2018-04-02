#include <controls/sequencer/window/CountMetronomeControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

CountMetronomeControls::CountMetronomeControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void CountMetronomeControls::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		ls.lock()->openScreen("metronomesound");
		break;
	}
}

void CountMetronomeControls::turnWheel(int i)
{
	init();
	if (param.compare("countin") == 0) {
		swGui->setCountIn(swGui->getCountInMode() + i);
	}
	else if (param.compare("inplay") == 0) {
		swGui->setInPlay(i > 0);
	}
	else if (param.compare("rate") == 0) {
		swGui->setRate(swGui->getRate() + i);
		sequencer.lock()->getActiveSequence().lock()->initMetaTracks();
	}
	else if (param.compare("inrec") == 0) {
		swGui->setInRec(i > 0);
	}
	else if (param.compare("waitforkey") == 0) {
		swGui->setWaitForKey(i > 0);
	}
}
