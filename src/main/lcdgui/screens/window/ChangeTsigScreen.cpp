#include <lcdgui/screens/window/ChangeTsigScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/TimeSignature.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeTsigScreen::ChangeTsigScreen(const int& layer)
	: ScreenComponent("changetsig", layer)
{
}

void ChangeTsigScreen::function(int i)
{
    BaseControls::function(i);
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	switch (i) {
	case 4:
		auto ts = swGui->getNewTimeSignature();
		auto sequence = sequencer.lock()->getActiveSequence().lock();
        sequence->setTimeSignature(swGui->getBar0(), swGui->getBar1(), ts->getNumerator(), ts->getDenominator());
        ls.lock()->openScreen("sequencer");
        break;
    }
}

void ChangeTsigScreen::turnWheel(int i)
{
    init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto seq = sequencer.lock()->getActiveSequence().lock();
	if (param.compare("bar0") == 0)
	{
		swGui->setBar0(swGui->getBar0() + i, seq->getLastBar());
	}
	else if (param.compare("bar1") == 0)
	{
		swGui->setBar1(swGui->getBar1() + i, seq->getLastBar());
	}
	else if (param.compare("newtsig") == 0 && i > 0)
	{
		swGui->getNewTimeSignature()->increase();
	}
	else if (param.compare("newtsig") == 0 && i < 0)
	{
		swGui->getNewTimeSignature()->decrease();
	}
}
