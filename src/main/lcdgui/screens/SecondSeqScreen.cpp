#include "SecondSeqScreen.hpp"

#include "PunchScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

SecondSeqScreen::SecondSeqScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "second-seq", layerIndex)
{
}

void SecondSeqScreen::open()
{
    displaySq();
    displayFunctionKeys();
}

void SecondSeqScreen::function(int i)
{
    auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));

    switch (i)
    {
    case 0: // Intentional fall-through
    case 1:
        punchScreen->tab = i;
        ls.lock()->openScreen(punchScreen->tabNames[i]);
        break;
    case 5:
        if (sequencer.lock()->isSecondSequenceEnabled())
        {
            sequencer.lock()->setSecondSequenceEnabled(false);
            ls.lock()->openScreen("sequencer");
            return;
        }
 
        sequencer.lock()->setSecondSequenceEnabled(true);
        ls.lock()->openScreen("sequencer");
        break;
    }
}

void SecondSeqScreen::turnWheel(int i)
{
	init();

	if (param.compare("sq") == 0)
		setSq(sq + i);
}

void SecondSeqScreen::setSq(int i)
{
    if (i < 0 || i > 99)
        return;
    
    sq = i;
    
    displaySq();
    displayFunctionKeys();
}

void SecondSeqScreen::displaySq()
{
    auto sqName = mpc.getSequencer().lock()->getSequence(sq).lock()->getName();
    findField("sq").lock()->setTextPadded(sq + 1, "0");
    findLabel("sequence-name").lock()->setText("-" + sqName);
}

void SecondSeqScreen::displayFunctionKeys()
{
    if (sequencer.lock()->isSecondSequenceEnabled())
        ls.lock()->setFunctionKeysArrangement(2);
    else
        ls.lock()->setFunctionKeysArrangement(0);
}
