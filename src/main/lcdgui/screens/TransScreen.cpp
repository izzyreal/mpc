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
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());

    setBar0(0);
    setBar1(sequencer.lock()->getSelectedSequence()->getLastBarIndex());

    displayTransposeAmount();
    displayTr();
}

void TransScreen::function(const int i)
{
    const auto punchScreen = mpc.screens->get<ScreenId::PunchScreen>();

    switch (i)
    {
        case 0: // Intentional fall-through
        case 2:
            punchScreen->setActiveTab(i);
            ls.lock()->openScreen(punchScreen->getTabNames()[i]);
            break;
        case 5:
            // if (amount == 0) break; // does 2kxl do that?
            openScreenById(ScreenId::TransposePermanentScreen);
            break;
        default:;
    }
}

void TransScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "tr")
    {
        setTr(tr + increment);
    }
    else if (focusedFieldName == "transpose-amount")
    {
        setTransposeAmount(transposeAmount + increment);
    }
    else if (focusedFieldName == "bar0")
    {
        const auto candidate = bar0 + increment;

        if (candidate < 0 ||
            candidate >
                sequencer.lock()->getSelectedSequence()->getLastBarIndex())
        {
            return;
        }

        setBar0(candidate);
    }
    else if (focusedFieldName == "bar1")
    {
        const auto candidate = bar1 + increment;

        if (candidate < 0 ||
            candidate >
                sequencer.lock()->getSelectedSequence()->getLastBarIndex())
        {
            return;
        }

        setBar1(candidate);
    }
}

void TransScreen::setTransposeAmount(const int i)
{
    transposeAmount = std::clamp(i, -12, 12);
    displayTransposeAmount();
}
int TransScreen::getTransposeAmount() const
{
    return transposeAmount;
}
int TransScreen::getTr() const
{
    return tr;
}

int TransScreen::getBar0() const
{
    return bar0;
}

int TransScreen::getBar1() const
{
    return bar1;
}

void TransScreen::setTr(const int8_t i)
{
    tr = std::clamp(i, ALL_TRACKS, Mpc2000XlSpecs::LAST_TRACK_INDEX);
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
        tr == ALL_TRACKS
            ? "ALL"
            : sequencer.lock()->getSelectedSequence()->getTrack(tr)->getName());
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
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());
}

void TransScreen::playStart()
{
    ScreenComponent::playStart();
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());
}

void TransScreen::rec()
{
    ScreenComponent::rec();
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());
}

void TransScreen::overDub()
{
    ScreenComponent::overDub();
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());
}

void TransScreen::stop()
{
    ScreenComponent::stop();
    findChild("function-keys")
        ->Hide(sequencer.lock()->getTransport()->isPlaying());
}
