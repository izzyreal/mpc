#include <controls/sequencer/window/ChangeBarsControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

ChangeBarsControls::ChangeBarsControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void ChangeBarsControls::function(int i)
{
    super::function(i);
	auto seq = sequence.lock();
	switch (i) {
	case 1:
		seq->insertBars(swGui->getChangeBarsNumberOfBars(), swGui->getChangeBarsAfterBar());
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		seq->deleteBars(swGui->getChangeBarsFirstBar(), swGui->getChangeBarsLastBar());
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void ChangeBarsControls::turnWheel(int i)
{
	init();
	auto seq = sequence.lock();
	if (param.compare("afterbar") == 0) {
		swGui->setChangeBarsAfterBar(swGui->getChangeBarsAfterBar() + i, seq->getLastBar());
	}
	else if (param.compare("numberofbars") == 0) {
		swGui->setChangeBarsNumberOfBars(swGui->getChangeBarsNumberOfBars() + i, seq->getLastBar());
	}
	else if (param.compare("firstbar") == 0) {
		swGui->setChangeBarsFirstBar(swGui->getChangeBarsFirstBar() + i, seq->getLastBar());
	}
	else if (param.compare("lastbar") == 0) {
		swGui->setChangeBarsLastBar(swGui->getChangeBarsLastBar() + i, seq->getLastBar());
	}
}
