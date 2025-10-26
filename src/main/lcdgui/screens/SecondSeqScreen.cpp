#include "SecondSeqScreen.hpp"

#include "PunchScreen.hpp"

using namespace mpc::lcdgui::screens;

SecondSeqScreen::SecondSeqScreen(mpc::Mpc &mpc, const int layerIndex)
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
    auto punchScreen = mpc.screens->get<PunchScreen>();

    switch (i)
    {
        case 0: // Intentional fall-through
        case 1:
            punchScreen->setActiveTab(i);
            mpc.getLayeredScreen()->openScreen(punchScreen->getTabNames()[i]);
            break;
        case 5:
            if (sequencer.lock()->isSecondSequenceEnabled())
            {
                sequencer.lock()->setSecondSequenceEnabled(false);
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
                return;
            }

            sequencer.lock()->setSecondSequenceEnabled(true);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}

void SecondSeqScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sq")
    {
        setSq(sq + i);
    }
}

void SecondSeqScreen::setSq(int i)
{
    sq = std::clamp(i, 0, 98);
    displaySq();
    displayFunctionKeys();
}

void SecondSeqScreen::displaySq()
{
    auto sqName = sequencer.lock()->getSequence(sq)->getName();
    findField("sq")->setTextPadded(sq + 1, "0");
    findLabel("sequence-name")->setText("-" + sqName);
}

void SecondSeqScreen::displayFunctionKeys()
{
    if (sequencer.lock()->isSecondSequenceEnabled())
    {
        ls->setFunctionKeysArrangement(2);
    }
    else
    {
        ls->setFunctionKeysArrangement(0);
    }
}
