#include "CountMetronomeScreen.hpp"

#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "count-metronome", layerIndex)
{
}

void CountMetronomeScreen::open()
{
    findField("in-play")->setAlignment(Alignment::Centered);
    findField("in-rec")->setAlignment(Alignment::Centered);
    findField("wait-for-key")->setAlignment(Alignment::Centered);

    displayCountIn();
    displayInPlay();
    displayRate();
    displayInRec();
    displayWaitForKey();
}

void CountMetronomeScreen::displayWaitForKey()
{
    findField("wait-for-key")->setText(waitForKey ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec()
{
    findField("in-rec")->setText(inRec ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate()
{
    findField("rate")->setText(rateNames[rate]);
}

void CountMetronomeScreen::displayInPlay()
{
    findField("in-play")->setText(inPlay ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn()
{
    findField("count-in")->setText(countInNames[countIn]);
}

void CountMetronomeScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 4:
            openScreenById(ScreenId::MetronomeSoundScreen);
            break;
    }
}

void CountMetronomeScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "count-in")
    {
        setCountIn(countIn + i);
    }
    else if (focusedFieldName == "in-play")
    {
        setInPlay(i > 0);
    }
    else if (focusedFieldName == "rate")
    {
        setRate(rate + i);
    }
    else if (focusedFieldName == "in-rec")
    {
        setInRec(i > 0);
    }
    else if (focusedFieldName == "wait-for-key")
    {
        setWaitForKey(i > 0);
    }
}

int CountMetronomeScreen::getCountInMode()
{
    return countIn;
}

void CountMetronomeScreen::setCountIn(int i)
{
    if (i < 0 || i > 2)
    {
        return;
    }

    countIn = i;
    displayCountIn();
}

void CountMetronomeScreen::setInPlay(bool b)
{
    if (inPlay == b)
    {
        return;
    }

    inPlay = b;
    displayInPlay();
}

bool CountMetronomeScreen::getInPlay()
{
    return inPlay;
}

int CountMetronomeScreen::getRate()
{
    return rate;
}

void CountMetronomeScreen::setRate(int i)
{
    if (i < 0 || i > 7)
    {
        return;
    }

    rate = i;
    displayRate();
}

void CountMetronomeScreen::setWaitForKey(bool b)
{
    if (waitForKey == b)
    {
        return;
    }

    waitForKey = b;
    displayWaitForKey();
}

bool CountMetronomeScreen::isWaitForKeyEnabled()
{
    return waitForKey;
}

void CountMetronomeScreen::setInRec(bool b)
{
    if (inRec == b)
    {
        return;
    }

    inRec = b;
    displayInRec();
}

bool CountMetronomeScreen::getInRec()
{
    return inRec;
}
