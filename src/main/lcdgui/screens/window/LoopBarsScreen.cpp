#include "LoopBarsScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;

LoopBarsScreen::LoopBarsScreen(Mpc &mpc, const int layerIndex)
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

void LoopBarsScreen::turnWheel(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();

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
        if (seq->getLastLoopBarIndex() == EndOfSequence)
        {
            seq->setLastLoopBarIndex(BarIndex(seq->getBarCount() - i));
        }
        else
        {
            seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - i);
        }
        displayLastBar();
        displayFirstBar();
        displayNumberOfBars();
    }
    else if (focusedFieldName == "numberofbars" && i < 0)
    {
        if (seq->getLastLoopBarIndex() == EndOfSequence)
        {
            seq->setLastLoopBarIndex(BarIndex(seq->getBarCount() - i));
        }
        else
        {
            seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - i);
        }

        displayFirstBar();
        displayLastBar();
        displayNumberOfBars();
    }
    else if (focusedFieldName == "numberofbars")
    {
        seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() + i);
        displayFirstBar();
        displayLastBar();
        displayNumberOfBars();
    }
}

void LoopBarsScreen::displayLastBar() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    if (seq->getLastLoopBarIndex() == EndOfSequence)
    {
        findField("lastbar")->setText("END");
    }
    else
    {
        findField("lastbar")->setText(
            std::to_string(seq->getLastLoopBarIndex() + 1));
    }
}

void LoopBarsScreen::displayNumberOfBars() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    findField("numberofbars")
        ->setText(std::to_string(seq->getLastLoopBarIndex() -
                                 seq->getFirstLoopBarIndex() + 1));
}

void LoopBarsScreen::displayFirstBar() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    findField("firstbar")
        ->setText(std::to_string(seq->getFirstLoopBarIndex() + 1));
}
