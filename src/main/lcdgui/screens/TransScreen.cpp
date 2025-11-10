#include "TransScreen.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "PunchScreen.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;

TransScreen::TransScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "trans", layerIndex)
{
}

void TransScreen::open()
{
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());

    setBar0(0);
    setBar1(sequencer->getActiveSequence()->getLastBarIndex());

    displayTransposeAmount();
    displayTr();
}

void TransScreen::function(int i)
{
    const auto punchScreen = mpc.screens->get<ScreenId::PunchScreen>();

    switch (i)
    {
        case 0: // Intentional fall-through
        case 2:
            punchScreen->setActiveTab(i);
            ls->openScreen(punchScreen->getTabNames()[i]);
            break;
        case 5:
            // if (amount == 0) break; // does 2kxl do that?
            openScreenById(ScreenId::TransposePermanentScreen);
            break;
    }
}

void TransScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        setTr(tr + i);
    }
    else if (focusedFieldName == "transpose-amount")
    {
        setTransposeAmount(transposeAmount + i);
    }
    else if (focusedFieldName == "bar0")
    {
        const auto candidate = bar0 + i;

        if (candidate < 0 ||
            candidate > sequencer->getActiveSequence()->getLastBarIndex())
        {
            return;
        }

        setBar0(candidate);
    }
    else if (focusedFieldName == "bar1")
    {
        const auto candidate = bar1 + i;

        if (candidate < 0 ||
            candidate > sequencer->getActiveSequence()->getLastBarIndex())
        {
            return;
        }

        setBar1(candidate);
    }
}

void TransScreen::setTransposeAmount(int i)
{
    transposeAmount = std::clamp(i, -12, 12);
    displayTransposeAmount();
}

void TransScreen::setTr(int i)
{
    tr = std::clamp(i, -1, 63);
    displayTr();
}

void TransScreen::setBar0(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    bar0 = i;

    if (bar0 > bar1)
    {
        bar1 = bar0;
    }

    displayBars();
}

void TransScreen::setBar1(int i)
{
    if (i < 0)
    {
        i = 0;
    }

    bar1 = i;

    if (bar1 < bar0)
    {
        bar0 = bar1;
    }

    displayBars();
}

void TransScreen::displayTransposeAmount() const
{
    findField("transpose-amount")->setTextPadded(transposeAmount);
}

void TransScreen::displayTr() const
{
    const auto trName = std::string(
        tr == -1 ? "ALL"
                 : sequencer->getActiveSequence()->getTrack(tr)->getName());
    findField("tr")->setTextPadded(tr + 1, "0");
    findLabel("track-name")->setText(trName);
}

void TransScreen::displayBars() const
{
    findField("bar0")->setTextPadded(std::to_string(bar0 + 1), "0");
    findField("bar1")->setTextPadded(std::to_string(bar1 + 1), "0");
}

void TransScreen::play()
{
    ScreenComponent::play();
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());
}

void TransScreen::playStart()
{
    ScreenComponent::playStart();
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());
}

void TransScreen::rec()
{
    ScreenComponent::rec();
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());
}

void TransScreen::overDub()
{
    ScreenComponent::overDub();
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());
}

void TransScreen::stop()
{
    ScreenComponent::stop();
    findChild("function-keys")->Hide(sequencer->getTransport()->isPlaying());
}
