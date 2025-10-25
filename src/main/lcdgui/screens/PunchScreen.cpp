#include "PunchScreen.hpp"

#include "sequencer/SeqUtil.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

PunchScreen::PunchScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "punch", layerIndex)
{
}

void PunchScreen::open()
{
    if (sequencer.lock()->isPlaying())
    {
        mpc.getLayeredScreen()->openScreen<TransScreen>();
        return;
    }

    if (tab != 0)
    {
        mpc.getLayeredScreen()->openScreen(tabNames[tab]);
        return;
    }

    auto lastTick = sequencer.lock()->getActiveSequence()->getLastTick();

    if (lastTick < sequencer.lock()->getPunchInTime() || lastTick < sequencer.lock()->getPunchOutTime() || (sequencer.lock()->getPunchInTime() == 0 && sequencer.lock()->getPunchOutTime() == 0))
    {
        sequencer.lock()->setPunchInTime(0);
        sequencer.lock()->setPunchOutTime(sequencer.lock()->getActiveSequence()->getLastTick());
    }

    displayBackground();
    displayAutoPunch();

    ls->setFunctionKeysArrangement(sequencer.lock()->isPunchEnabled());
}

void PunchScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "auto-punch")
    {
        setAutoPunch(sequencer.lock()->getAutoPunchMode() + i);
    }

    checkAllTimes(mpc, i);
    
    // Sync punch times with Sequencer
    sequencer.lock()->setPunchInTime(time0);
    sequencer.lock()->setPunchOutTime(time1);
}

void PunchScreen::function(int i)
{

    switch (i)
    {
        case 1: // Intentional fall-through
        case 2:
            tab = i;
            mpc.getLayeredScreen()->openScreen(tabNames[i]);
            break;
        case 5:
            sequencer.lock()->setPunchEnabled(!sequencer.lock()->isPunchEnabled());
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}

void PunchScreen::setAutoPunch(int i)
{
    if (i < 0 || i > 2)
    {
        return;
    }

    sequencer.lock()->setAutoPunchMode(i);

    displayAutoPunch();
    displayTime();
    displayBackground();
}

void PunchScreen::displayAutoPunch()
{
    findField("auto-punch")->setText(autoPunchNames[sequencer.lock()->getAutoPunchMode()]);
}

void PunchScreen::displayTime()
{
    auto sequence = sequencer.lock()->getActiveSequence().get();

    for (int i = 0; i < 3; i++)
    {
        findField("time" + std::to_string(i))->Hide(sequencer.lock()->getAutoPunchMode() == 1);
        findLabel("time" + std::to_string(i))->Hide(sequencer.lock()->getAutoPunchMode() == 1);
        findField("time" + std::to_string(i + 3))->Hide(sequencer.lock()->getAutoPunchMode() == 0);
        findLabel("time" + std::to_string(i + 3))->Hide(sequencer.lock()->getAutoPunchMode() == 0);
    }

    findLabel("time3")->Hide(sequencer.lock()->getAutoPunchMode() != 2);

    findField("time0")->setTextPadded(SeqUtil::getBar(sequence, sequencer.lock()->getPunchInTime()) + 1, "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, sequencer.lock()->getPunchInTime()) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence, sequencer.lock()->getPunchInTime()), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(sequence, sequencer.lock()->getPunchOutTime()) + 1, "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, sequencer.lock()->getPunchOutTime()) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence, sequencer.lock()->getPunchOutTime()), "0");
}

void PunchScreen::displayBackground()
{
    std::string bgName = "punch-in";

    if (sequencer.lock()->getAutoPunchMode() == 1)
    {
        bgName = "punch-out";
    }
    else if (sequencer.lock()->getAutoPunchMode() == 2)
    {
        bgName = "punch-in-out";
    }

    findBackground()->setBackgroundName(bgName);
}

void PunchScreen::rec()
{
    mpc.getLayeredScreen()->openScreen<SequencerScreen>();
    ScreenComponent::rec();
}

void PunchScreen::overDub()
{
    mpc.getLayeredScreen()->openScreen<SequencerScreen>();
    ScreenComponent::rec();
}
