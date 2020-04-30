#include <controls/sequencer/window/ChangeTsigControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/TimeSignature.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

ChangeTsigControls::ChangeTsigControls()
	: AbstractSequencerControls()
{
}

void ChangeTsigControls::function(int i)
{
    super::function(i);
	switch (i) {
	case 4:
		auto ts = swGui->getNewTimeSignature();
        sequence.lock()->setTimeSignature(swGui->getBar0(), swGui->getBar1(), ts->getNumerator(), ts->getDenominator());
        ls.lock()->openScreen("sequencer");
        break;
    }
}

void ChangeTsigControls::turnWheel(int i)
{
    init();
	auto seq = sequence.lock();
	if (param.compare("bar0") == 0) {
		swGui->setBar0(swGui->getBar0() + i, seq->getLastBar());
	}
	else if (param.compare("bar1") == 0) {
		swGui->setBar1(swGui->getBar1() + i, seq->getLastBar());
	}
	else if (param.compare("newtsig") == 0 && i > 0) {
		swGui->getNewTimeSignature()->increase();
	}
	else if (param.compare("newtsig") == 0 && i < 0) {
		swGui->getNewTimeSignature()->decrease();
	}
}
