#include "LoopBarsScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;

LoopBarsScreen::LoopBarsScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop-bars-window", layerIndex)
{
}

void LoopBarsScreen::open()
{
    findField("firstbar")->setAlignment(Alignment::Centered);
    findField("lastbar")->setAlignment(Alignment::Centered);
    findField("numberofbars")->setAlignment(Alignment::Centered);
    displayFirstBar();
    displayLastBar();
    displayNumberOfBars();
}

void LoopBarsScreen::turnWheel(int i)
{
    const auto seq = sequencer->getActiveSequence();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "firstbar")
    {
        seq->setFirstLoopBarIndex(seq->getFirstLoopBarIndex() + i);
        displayFirstBar();
        displayLastBar();
        displayNumberOfBars();
    }
    else if (focusedFieldName == "lastbar")
    {
        seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() + i);
        displayLastBar();
        displayFirstBar();
        displayNumberOfBars();
    }
    else if (focusedFieldName == "numberofbars" && i < 0)
    {
        if (seq->isLastLoopBarEnd())
        {
            seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - 2);
            displayLastBar();
            displayNumberOfBars();
        }
        else if (seq->getLastLoopBarIndex() >= seq->getFirstLoopBarIndex())
        {
            seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - 1);
            displayLastBar();
            displayNumberOfBars();
        }
    }
    else if (focusedFieldName == "numberofbars")
    {
        seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() + i);
        displayLastBar();
        displayNumberOfBars();
    }
}

void LoopBarsScreen::displayLastBar()
{
    const auto seq = sequencer->getActiveSequence();

    if (seq->isLastLoopBarEnd())
    {
        findField("lastbar")->setText("END");
    }
    else
    {
        findField("lastbar")->setText(
            std::to_string(seq->getLastLoopBarIndex() + 1));
    }
}

void LoopBarsScreen::displayNumberOfBars()
{
    const auto seq = sequencer->getActiveSequence();
    findField("numberofbars")
        ->setText(std::to_string(seq->getLastLoopBarIndex() -
                                 seq->getFirstLoopBarIndex() + 1));
}

void LoopBarsScreen::displayFirstBar()
{
    const auto seq = sequencer->getActiveSequence();
    findField("firstbar")
        ->setText(std::to_string(seq->getFirstLoopBarIndex() + 1));
}
