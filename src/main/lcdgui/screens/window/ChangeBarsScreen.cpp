#include "ChangeBarsScreen.hpp"
#include "sequencer/Transport.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;

ChangeBarsScreen::ChangeBarsScreen(Mpc &mpc, const int layerIndex)
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

void ChangeBarsScreen::function(const int i)
{
    ScreenComponent::function(i);
    const auto seq = sequencer.lock()->getSelectedSequence();

    switch (i)
    {
        case 1:
        {
            if (numberOfBars > 0 && afterBar <= seq->getLastBarIndex())
            {
                sequencer.lock()->getTransport()->setPosition(0);
            }

            sequencer.lock()->copySelectedSequenceToUndoSequence();
            seq->insertBars(numberOfBars, BarIndex(afterBar));

            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        case 4:
        {
            if (firstBar <= seq->getLastBarIndex())
            {
                sequencer.lock()->getTransport()->setPosition(0);
            }

            sequencer.lock()->copySelectedSequenceToUndoSequence();
            seq->deleteBars(firstBar, lastBar);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void ChangeBarsScreen::turnWheel(const int i)
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

void ChangeBarsScreen::displayAfterBar() const
{
    findField("afterbar")->setTextPadded(afterBar);
}

void ChangeBarsScreen::displayNumberOfBars() const
{
    findField("numberofbars")->setTextPadded(numberOfBars);
}

void ChangeBarsScreen::displayFirstBar() const
{
    findField("firstbar")->setTextPadded(firstBar + 1);
}

void ChangeBarsScreen::displayLastBar() const
{
    findField("lastbar")->setTextPadded(lastBar + 1);
}

void ChangeBarsScreen::setLastBar(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    lastBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0));

    if (lastBar < firstBar)
    {
        setFirstBar(lastBar);
    }

    displayLastBar();
}

void ChangeBarsScreen::setFirstBar(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    firstBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0));

    displayFirstBar();

    if (firstBar > lastBar)
    {
        setLastBar(firstBar);
    }
}

void ChangeBarsScreen::setNumberOfBars(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    numberOfBars = std::clamp(i, 0, 998 - std::max(seq->getLastBarIndex(), 0));
    displayNumberOfBars();
}

void ChangeBarsScreen::setAfterBar(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    afterBar = std::clamp(i, 0, std::max(seq->getLastBarIndex(), 0) + 1);
    displayAfterBar();
}
