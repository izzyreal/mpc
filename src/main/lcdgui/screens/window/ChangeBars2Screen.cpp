#include <lcdgui/screens/window/ChangeBars2Screen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeBars2Screen::ChangeBars2Screen(const int& layer)
	: ScreenComponent("changebars2", layer)
{
}

void ChangeBars2Screen::function(int i)
{
	BaseControls::function(i);
	auto seq = sequencer.lock()->getActiveSequence().lock();

	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

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

void ChangeBars2Screen::turnWheel(int i)
{
	init();

	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("newbars") == 0)
	{
		swGui->setNewBars(swGui->getNewBars() + i);
	}
}
