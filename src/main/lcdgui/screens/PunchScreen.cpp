#include "PunchScreen.hpp"

#include <sequencer/SeqUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace std;

PunchScreen::PunchScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "punch", layerIndex)
{
}

void PunchScreen::open()
{
    if (sequencer.lock()->isPlaying())
    {
        openScreen("trans");
        return;
    }

    if (tab != 0)
    {
        openScreen(tabNames[tab]);
        return;
    }

    if (sequencer.lock()->getActiveSequence().lock()->getLastTick() < time0 || (time0 == 0 && time1 == 0))
    {
        setTime0(0);
        setTime1(sequencer.lock()->getActiveSequence().lock()->getLastTick());
    }

    displayBackground();
    displayAutoPunch();

    ls.lock()->setFunctionKeysArrangement(on);
}

void PunchScreen::turnWheel(int i)
{
    init();
    
	if (param.compare("auto-punch") == 0)
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
        openScreen(tabNames[i]);
        break;
    case 5:
        on = !on;
        openScreen("sequencer");
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
    findField("auto-punch").lock()->setText(autoPunchNames[autoPunch]);
}

void PunchScreen::displayTime()
{
    auto ls = mpc.getLayeredScreen().lock();

    auto sequence = sequencer.lock()->getActiveSequence().lock().get();

    for (int i = 0; i < 3; i++)
    {
        findField("time" + to_string(i)).lock()->Hide(autoPunch == 1);
        findLabel("time" + to_string(i)).lock()->Hide(autoPunch == 1);
        findField("time" + to_string(i + 3)).lock()->Hide(autoPunch == 0);
        findLabel("time" + to_string(i + 3)).lock()->Hide(autoPunch == 0);
    }

    findLabel("time3").lock()->Hide(autoPunch != 2);

    findField("time0").lock()->setTextPadded(SeqUtil::getBar(sequence, time0) + 1, "0");
    findField("time1").lock()->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
    findField("time2").lock()->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
    findField("time3").lock()->setTextPadded(SeqUtil::getBar(sequence, time1) + 1, "0");
    findField("time4").lock()->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
    findField("time5").lock()->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void PunchScreen::displayBackground()
{
    string bgName = "punch-in";
    
    if (autoPunch == 1)
    {
        bgName = "punch-out";
    }
    else if (autoPunch == 2) {
        bgName = "punch-in-out";
    }
    
    findBackground().lock()->setName(bgName);
}

void PunchScreen::rec()
{
    openScreen("sequencer");
    ScreenComponent::rec();
}

void PunchScreen::overDub()
{
    openScreen("sequencer");
    ScreenComponent::rec();
}
