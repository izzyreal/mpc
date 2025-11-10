#include "ChangeTsigScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;

ChangeTsigScreen::ChangeTsigScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "change-tsig", layerIndex)
{
}

void ChangeTsigScreen::open()
{
    timesignature = sequencer->getActiveSequence()->getTimeSignature();

    bar0 = 0;
    bar1 = sequencer->getActiveSequence()->getLastBarIndex();

    displayBars();
    displayNewTsig();
}

void ChangeTsigScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
            const auto sequence = sequencer->getActiveSequence();

            const auto barLengths = sequence->getBarLengthsInTicks();

            sequence->setTimeSignature(bar0, bar1, timesignature.getNumerator(),
                                       timesignature.getDenominator());

            const auto &newBarLengths = sequence->getBarLengthsInTicks();

            for (int j = 0; j < barLengths.size(); j++)
            {
                if (barLengths[j] != newBarLengths[j])
                {
                    sequencer->getTransport()->setPosition(
                        0); // Only reset sequencer position when a
                            // bar length has changed
                    break;
                }
            }

            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}

void ChangeTsigScreen::turnWheel(const int i)
{

    const auto seq = sequencer->getActiveSequence();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "bar0")
    {
        setBar0(bar0 + i, seq->getLastBarIndex());
        displayBars();
    }
    else if (focusedFieldName == "bar1")
    {
        setBar1(bar1 + i, seq->getLastBarIndex());
        displayBars();
    }
    else if (focusedFieldName == "newtsig" && i > 0)
    {
        timesignature.increase();
        displayNewTsig();
    }
    else if (focusedFieldName == "newtsig" && i < 0)
    {
        timesignature.decrease();
        displayNewTsig();
    }
}

void ChangeTsigScreen::displayBars() const
{
    findField("bar0")->setTextPadded(bar0 + 1);
    findField("bar1")->setTextPadded(bar1 + 1);
}

void ChangeTsigScreen::displayNewTsig() const
{
    if (ls->getCurrentScreenName() == "delete-sequence")
    {
        return;
    }

    const auto result =
        StrUtil::padLeft(std::to_string(timesignature.getNumerator()), " ", 2) +
        "/" +
        StrUtil::padLeft(std::to_string(timesignature.getDenominator()), " ",
                         2);
    findField("newtsig")->setText(result);
}

void ChangeTsigScreen::setBar0(const int i, const int max)
{
    bar0 = std::clamp(i, 0, max);

    if (bar0 > bar1)
    {
        bar1 = bar0;
    }

    displayBars();
}

void ChangeTsigScreen::setBar1(const int i, const int max)
{
    bar1 = std::clamp(i, 0, max);

    if (bar1 < bar0)
    {
        bar0 = bar1;
    }

    displayBars();
}
