#include <lcdgui/screens/window/ChangeBarsScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeBarsScreen::ChangeBarsScreen(const int& layer)
	: ScreenComponent("changebars", layer)
{
}

void ChangeBarsScreen::function(int i)
{
    BaseControls::function(i);
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
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

void ChangeBarsScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto seq = sequencer.lock()->getActiveSequence().lock();
	if (param.compare("afterbar") == 0)
	{
		swGui->setChangeBarsAfterBar(swGui->getChangeBarsAfterBar() + i, seq->getLastBar());
	}
	else if (param.compare("numberofbars") == 0)
	{
		swGui->setChangeBarsNumberOfBars(swGui->getChangeBarsNumberOfBars() + i, seq->getLastBar());
	}
	else if (param.compare("firstbar") == 0)
	{
		swGui->setChangeBarsFirstBar(swGui->getChangeBarsFirstBar() + i, seq->getLastBar());
	}
	else if (param.compare("lastbar") == 0)
	{
		swGui->setChangeBarsLastBar(swGui->getChangeBarsLastBar() + i, seq->getLastBar());
	}
}
