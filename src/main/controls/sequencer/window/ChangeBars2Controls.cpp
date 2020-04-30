#include <controls/sequencer/window/ChangeBars2Controls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

ChangeBars2Controls::ChangeBars2Controls()
	: AbstractSequencerControls()
{
}

void ChangeBars2Controls::function(int i)
{
	super::function(i);
	auto seq = sequence.lock();
	switch (i) {
	case 2:
		ls.lock()->openScreen("sequencer");
		ls.lock()->openScreen("changebars");
		break;
	case 4:
		if (swGui->getNewBars() < seq->getLastBar()) {
			seq->deleteBars(swGui->getNewBars() + 1, seq->getLastBar());
		}
		if (swGui->getNewBars() > seq->getLastBar()) {
			seq->insertBars(swGui->getNewBars() - seq->getLastBar(), seq->getLastBar() + 1);
		}
		ls.lock()->openScreen("sequencer");
		sequencer.lock()->setBar(0);
		break;
	}
}

void ChangeBars2Controls::turnWheel(int i)
{
	init();
	if (param.compare("newbars") == 0)
		swGui->setNewBars(swGui->getNewBars() + i);
}
