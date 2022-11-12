#include "SecondSeqScreen.hpp"

#include "PunchScreen.hpp"

using namespace mpc::lcdgui::screens;

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
    auto punchScreen = mpc.screens->get<PunchScreen>("punch");

    switch (i)
    {
    case 0: // Intentional fall-through
    case 1:
        punchScreen->tab = i;
        openScreen(punchScreen->tabNames[i]);
        break;
    case 5:
        if (sequencer->isSecondSequenceEnabled())
        {
            sequencer->setSecondSequenceEnabled(false);
            openScreen("sequencer");
            return;
        }
 
        sequencer->setSecondSequenceEnabled(true);
        openScreen("sequencer");
        break;
    }
}

void SecondSeqScreen::turnWheel(int i)
{
	init();

	if (param == "sq")
		setSq(sq + i);
}

void SecondSeqScreen::setSq(int i)
{
    if (i < 0 || i > 98)
        return;
    
    sq = i;
    
    displaySq();
    displayFunctionKeys();
}

void SecondSeqScreen::displaySq()
{
    auto sqName = sequencer->getSequence(sq)->getName();
    findField("sq")->setTextPadded(sq + 1, "0");
    findLabel("sequence-name")->setText("-" + sqName);
}

void SecondSeqScreen::displayFunctionKeys()
{
    if (sequencer->isSecondSequenceEnabled())
        ls.lock()->setFunctionKeysArrangement(2);
    else
        ls.lock()->setFunctionKeysArrangement(0);
}
