#include "ChangeBarsScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;

ChangeBarsScreen::ChangeBarsScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "change-bars", layerIndex)
{
}

void ChangeBarsScreen::open()
{
    setAfterBar(0);
    setNumberOfBars(0);
    setFirstBar(0);
    setLastBar(0);
}

void ChangeBarsScreen::function(int i)
{
    ScreenComponent::function(i);
    const auto seq = sequencer->getActiveSequence();

    switch (i)
    {
        case 1:
        {
            if (numberOfBars > 0 && afterBar <= seq->getLastBarIndex())
            {
                sequencer->move(0);
            }

            seq->insertBars(numberOfBars, afterBar);

            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        case 4:
        {
            if (firstBar <= seq->getLastBarIndex())
            {
                sequencer->move(0);
            }

            seq->deleteBars(firstBar, lastBar);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
    }
}

void ChangeBarsScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "afterbar")
    {
        setAfterBar(afterBar + i);
    }
    else if (focusedFieldName == "numberofbars")
    {
        setNumberOfBars(numberOfBars + i);
    }
    else if (focusedFieldName == "firstbar")
    {
        setFirstBar(firstBar + i);
    }
    else if (focusedFieldName == "lastbar")
    {
        setLastBar(lastBar + i);
    }
}

void ChangeBarsScreen::displayAfterBar()
{
    findField("afterbar")->setTextPadded(afterBar);
}

void ChangeBarsScreen::displayNumberOfBars()
{
    findField("numberofbars")->setTextPadded(numberOfBars);
}

void ChangeBarsScreen::displayFirstBar()
{
    findField("firstbar")->setTextPadded(firstBar + 1);
}

void ChangeBarsScreen::displayLastBar()
{
    findField("lastbar")->setTextPadded(lastBar + 1);
}

void ChangeBarsScreen::setLastBar(int i)
{
    const auto seq = sequencer->getActiveSequence();

    lastBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0));

    if (lastBar < firstBar)
    {
        setFirstBar(lastBar);
    }

    displayLastBar();
}

void ChangeBarsScreen::setFirstBar(int i)
{
    const auto seq = sequencer->getActiveSequence();

    firstBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0));

    displayFirstBar();

    if (firstBar > lastBar)
    {
        setLastBar(firstBar);
    }
}

void ChangeBarsScreen::setNumberOfBars(int i)
{
    const auto seq = sequencer->getActiveSequence();
    numberOfBars = std::clamp(i, 0, 998 - std::max(seq->getLastBarIndex(), 0));
    displayNumberOfBars();
}

void ChangeBarsScreen::setAfterBar(int i)
{
    const auto seq = sequencer->getActiveSequence();
    afterBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0) + 1);
    displayAfterBar();
}
