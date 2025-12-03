#include "ChangeTsigScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::window;

ChangeTsigScreen::ChangeTsigScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "change-tsig", layerIndex)
{
}

void ChangeTsigScreen::open()
{
    const auto seq = sequencer.lock();
    timesignature = seq->getSelectedSequence()->getTimeSignature(
        seq->getTransport()->getTickPosition());

    bar0 = 0;
    bar1 = seq->getSelectedSequence()->getLastBarIndex();

    displayBars();
    displayNewTsig();
}

void ChangeTsigScreen::function(const int i)
{
    ScreenComponent::function(i);

    if (i != 4)
    {
        return;
    }

    const auto sequence = sequencer.lock()->getSelectedSequence();

    sequence->setTimeSignature(bar0, bar1, timesignature.numerator,
                               timesignature.denominator);

    sequencer.lock()->getTransport()->setPosition(0);
    openScreenById(ScreenId::SequencerScreen);
}

void ChangeTsigScreen::turnWheel(const int i)
{
    const auto seq = sequencer.lock()->getSelectedSequence();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "bar0")
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
    if (ls.lock()->getCurrentScreenName() == "delete-sequence")
    {
        return;
    }

    const auto result =
        StrUtil::padLeft(std::to_string(timesignature.numerator), " ", 2) +
        "/" +
        StrUtil::padLeft(std::to_string(timesignature.denominator), " ", 2);
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
