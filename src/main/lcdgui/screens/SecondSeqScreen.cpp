#include "SecondSeqScreen.hpp"

#include "Mpc.hpp"
#include "PunchScreen.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens;

SecondSeqScreen::SecondSeqScreen(Mpc &mpc, const int layerIndex)
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
    const auto punchScreen = mpc.screens->get<ScreenId::PunchScreen>();

    switch (i)
    {
        case 0: // Intentional fall-through
        case 1:
            punchScreen->setActiveTab(i);
            ls.lock()->openScreen(punchScreen->getTabNames()[i]);
            break;
        case 5:
            if (sequencer.lock()->isSecondSequenceEnabled())
            {
                sequencer.lock()->setSecondSequenceEnabled(false);
                openScreenById(ScreenId::SequencerScreen);
                return;
            }

            sequencer.lock()->setSecondSequenceEnabled(true);
            openScreenById(ScreenId::SequencerScreen);
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

void SecondSeqScreen::displaySq() const
{
    const auto sqName = sequencer.lock()->getSequence(sq)->getName();
    findField("sq")->setTextPadded(sq + 1, "0");
    findLabel("sequence-name")->setText("-" + sqName);
}

void SecondSeqScreen::displayFunctionKeys() const
{
    if (sequencer.lock()->isSecondSequenceEnabled())
    {
        setFunctionKeysArrangement(2);
    }
    else
    {
        setFunctionKeysArrangement(0);
    }
}
