#include "PunchScreen.hpp"

#include <sequencer/SeqUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

PunchScreen::PunchScreen(mpc::Mpc& mpc, const int layerIndex)
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
    
    if (lastTick < time0 || lastTick < time1 || (time0 == 0 && time1 == 0))
    {
        setTime0(0);
        setTime1(sequencer.lock()->getActiveSequence()->getLastTick());
    }

    displayBackground();
    displayAutoPunch();

    ls->setFunctionKeysArrangement(on);
}

void PunchScreen::turnWheel(int i)
{
    init();
    
	if (param == "auto-punch")
		setAutoPunch(autoPunch + i);

    checkAllTimes(mpc, i);
}

void PunchScreen::function(int i)
{
    init();
	
    switch (i)
    {
    case 1: // Intentional fall-through
    case 2:
        tab = i;
        mpc.getLayeredScreen()->openScreen(tabNames[i]);
        break;
    case 5:
        on = !on;
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        break;
    }
}

void PunchScreen::setAutoPunch(int i)
{
    if (i < 0 || i > 2)
        return;

    autoPunch = i;

    displayAutoPunch();
    displayTime();
    displayBackground();
}

void PunchScreen::displayAutoPunch()
{
    findField("auto-punch")->setText(autoPunchNames[autoPunch]);
}

void PunchScreen::displayTime()
{
    auto sequence = sequencer.lock()->getActiveSequence().get();

    for (int i = 0; i < 3; i++)
    {
        findField("time" + std::to_string(i))->Hide(autoPunch == 1);
        findLabel("time" + std::to_string(i))->Hide(autoPunch == 1);
        findField("time" + std::to_string(i + 3))->Hide(autoPunch == 0);
        findLabel("time" + std::to_string(i + 3))->Hide(autoPunch == 0);
    }

    findLabel("time3")->Hide(autoPunch != 2);

    findField("time0")->setTextPadded(SeqUtil::getBar(sequence, time0) + 1, "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(sequence, time1) + 1, "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void PunchScreen::displayBackground()
{
    std::string bgName = "punch-in";
    
    if (autoPunch == 1)
    {
        bgName = "punch-out";
    }
    else if (autoPunch == 2) {
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
