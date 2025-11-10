#include "PunchScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

PunchScreen::PunchScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "punch", layerIndex)
{
}

void PunchScreen::open()
{
    if (sequencer->getTransport()->isPlaying())
    {
        openScreenById(ScreenId::TransScreen);
        return;
    }

    if (tab != 0)
    {
        ls->openScreen(tabNames[tab]);
        return;
    }

    const auto lastTick = sequencer->getActiveSequence()->getLastTick();

    if (lastTick < sequencer->getTransport()->getPunchInTime() ||
        lastTick < sequencer->getTransport()->getPunchOutTime() ||
        (sequencer->getTransport()->getPunchInTime() == 0 && sequencer->getTransport()->getPunchOutTime() == 0))
    {
        sequencer->getTransport()->setPunchInTime(0);
        sequencer->getTransport()->setPunchOutTime(
            sequencer->getActiveSequence()->getLastTick());
        setTime1(sequencer->getActiveSequence()->getLastTick());
    }

    setTime0(sequencer->getTransport()->getPunchInTime());
    setTime1(sequencer->getTransport()->getPunchOutTime());

    displayBackground();
    displayAutoPunch();

    ls->setFunctionKeysArrangement(sequencer->getTransport()->isPunchEnabled());
}

void PunchScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "auto-punch")
    {
        setAutoPunch(sequencer->getTransport()->getAutoPunchMode() + i);
        return;
    }

    checkAllTimes(mpc, i);

    sequencer->getTransport()->setPunchInTime(time0);
    sequencer->getTransport()->setPunchOutTime(time1);
}

void PunchScreen::function(int i)
{

    switch (i)
    {
        case 1: // Intentional fall-through
        case 2:
            tab = i;
            ls->openScreen(tabNames[i]);
            break;
        case 5:
            sequencer->getTransport()->setPunchEnabled(!sequencer->getTransport()->isPunchEnabled());
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}

void PunchScreen::setAutoPunch(int i)
{
    sequencer->getTransport()->setAutoPunchMode(std::clamp(i, 0, 2));

    displayAutoPunch();
    displayTime();
    displayBackground();
}

void PunchScreen::displayAutoPunch() const
{
    findField("auto-punch")
        ->setText(autoPunchNames[sequencer->getTransport()->getAutoPunchMode()]);
}

void PunchScreen::displayTime()
{
    const auto sequence = sequencer->getActiveSequence().get();

    for (int i = 0; i < 3; i++)
    {
        findField("time" + std::to_string(i))
            ->Hide(sequencer->getTransport()->getAutoPunchMode() == 1);
        findLabel("time" + std::to_string(i))
            ->Hide(sequencer->getTransport()->getAutoPunchMode() == 1);
        findField("time" + std::to_string(i + 3))
            ->Hide(sequencer->getTransport()->getAutoPunchMode() == 0);
        findLabel("time" + std::to_string(i + 3))
            ->Hide(sequencer->getTransport()->getAutoPunchMode() == 0);
    }

    findLabel("time3")->Hide(sequencer->getTransport()->getAutoPunchMode() != 2);

    findField("time0")->setTextPadded(SeqUtil::getBar(sequence, time0) + 1,
                                      "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1,
                                      "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(sequence, time1) + 1,
                                      "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1,
                                      "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void PunchScreen::displayBackground()
{
    std::string bgName = "punch-in";

    if (sequencer->getTransport()->getAutoPunchMode() == 1)
    {
        bgName = "punch-out";
    }
    else if (sequencer->getTransport()->getAutoPunchMode() == 2)
    {
        bgName = "punch-in-out";
    }

    findBackground()->setBackgroundName(bgName);
}

void PunchScreen::rec()
{
    openScreenById(ScreenId::SequencerScreen);
    ScreenComponent::rec();
}

void PunchScreen::overDub()
{
    openScreenById(ScreenId::SequencerScreen);
    ScreenComponent::rec();
}

int PunchScreen::getActiveTab() const
{
    return tab;
}

void PunchScreen::setActiveTab(const int tabToUse)
{
    tab = tabToUse;
}

const std::vector<std::string> &PunchScreen::getTabNames() const
{
    return tabNames;
}
