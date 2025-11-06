#include "CountMetronomeScreen.hpp"

#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(Mpc &mpc, const int layerIndex)
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

void CountMetronomeScreen::displayWaitForKey() const
{
    findField("wait-for-key")->setText(waitForKey ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec() const
{
    findField("in-rec")->setText(inRec ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate() const
{
    findField("rate")->setText(rateNames[rate]);
}

void CountMetronomeScreen::displayInPlay() const
{
    findField("in-play")->setText(inPlay ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn() const
{
    findField("count-in")->setText(countInNames[countIn]);
}

void CountMetronomeScreen::function(const int i)
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

void CountMetronomeScreen::turnWheel(const int i)
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

int CountMetronomeScreen::getCountInMode() const
{
    return countIn;
}

void CountMetronomeScreen::setCountIn(const int i)
{
    if (i < 0 || i > 2)
    {
        return;
    }

    countIn = i;
    displayCountIn();
}

void CountMetronomeScreen::setInPlay(const bool b)
{
    if (inPlay == b)
    {
        return;
    }

    inPlay = b;
    displayInPlay();
}

bool CountMetronomeScreen::getInPlay() const
{
    return inPlay;
}

int CountMetronomeScreen::getRate() const
{
    return rate;
}

void CountMetronomeScreen::setRate(const int i)
{
    if (i < 0 || i > 7)
    {
        return;
    }

    rate = i;
    displayRate();
}

void CountMetronomeScreen::setWaitForKey(const bool b)
{
    if (waitForKey == b)
    {
        return;
    }

    waitForKey = b;
    displayWaitForKey();
}

bool CountMetronomeScreen::isWaitForKeyEnabled() const
{
    return waitForKey;
}

void CountMetronomeScreen::setInRec(const bool b)
{
    if (inRec == b)
    {
        return;
    }

    inRec = b;
    displayInRec();
}

bool CountMetronomeScreen::getInRec() const
{
    return inRec;
}
