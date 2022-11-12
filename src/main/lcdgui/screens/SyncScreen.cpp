#include "SyncScreen.hpp"

using namespace mpc::lcdgui::screens;

SyncScreen::SyncScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "sync", layerIndex)
{
}

void SyncScreen::open()
{
    if (tab == 2 && ls.lock()->getPreviousScreenName() == "midi-sw")
        tab = 0;
    
    if (tab == 2)
    {
        openScreen("midi-sw");
        return;
    }
    
    displayIn();
    displayOut();
    displayModeIn();
    displayModeOut();
    displayReceiveMMC();
    displaySendMMC();
    displayShiftEarly();
}

void SyncScreen::turnWheel(int i)
{
    init();
    
    if (param == "in")
    {
        setIn(in + i);
    }
    else if (param == "out")
    {
        setOut(out + i);
    }
    else if (param == "mode-in")
    {
        setModeIn(modeIn + i);
    }
    else if (param == "mode-out")
    {
        setModeOut(getModeOut() + i);
    }
    else if (param == "shift-early")
    {
        if (modeIn == 1) {
            setShiftEarly(shiftEarly + i);
        }
        else {
            setFrameRate(frameRate + i);
        }
    }
    else if (param == "receive-mmc")
    {
        setReceiveMMCEnabled(i > 0);
    }
    else if (param == "send-mmc")
    {
        setSendMMCEnabled(i > 0);
    }
}

void SyncScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 2:
            tab = 2;
            openScreen("midi-sw");
            break;
    }
}

void SyncScreen::setIn(int i)
{
    if (i < 0 || i > 1)
        return;

    in = i;
    displayIn();
}

void SyncScreen::setOut(int i)
{
    if (i < 0 || i > 2)
        return;
    
    out = i;
    displayOut();
}

void SyncScreen::setShiftEarly(int i)
{
    if (i < 0 || i > 20)
        return;
    
    shiftEarly = i;
    displayShiftEarly();
}

void SyncScreen::setSendMMCEnabled(bool b)
{
    sendMMCEnabled = b;
    displaySendMMC();
}

void SyncScreen::setFrameRate(int i)
{
    if (i < 0 || i > 29) // What are the real bounds on a2KXL?
        return;
    
    frameRate = i;
    displayShiftEarly();
}

void SyncScreen::setModeIn(unsigned char c)
{
    if (c < 0 || c > 2)
    {
        return;
    }
    
    modeIn = c;
    displayModeIn();
    displayShiftEarly();
}

int SyncScreen::getModeOut()
{
    return modeOut;
}

void SyncScreen::setModeOut(unsigned char c)
{
    if (c < 0 || c > 2)
    {
        return;
    }

    modeOut = c;

    displayModeOut();
}

void SyncScreen::setReceiveMMCEnabled(bool b)
{
    receiveMMCEnabled = b;
    displayReceiveMMC();
}

// Also used to display "Frame rate:" field}
void SyncScreen::displayShiftEarly()
{
    if (modeIn == 0) {
        findLabel("shift-early")->Hide(true);
        findField("shift-early")->Hide(true);
    }
    else if (modeIn == 1)
    {
        auto label = findLabel("shift-early");
        auto field = findField("shift-early");
        label->Hide(false);
        field->Hide(false);
        field->setLocation(100, field->getY());
        label->setText("Shift early(ms):");
        field->setTextPadded(shiftEarly);
    }
    else if (modeIn == 2)
    {
        auto label = findLabel("shift-early");
        auto field = findField("shift-early");
        label->Hide(false);
        field->Hide(false);
        field->setLocation(70, field->getY());
        label->setText("Frame rate:");
        field->setTextPadded(frameRate);
    }
}

void SyncScreen::displayIn()
{
    auto result = std::to_string(in + 1);
    findField("in")->setText(result);
}

void SyncScreen::displayOut()
{
    auto outText = " A";
    
    if (out == 1)
    {
        outText = " B";
    }
    else if (out == 2)
    {
        outText = "A/B";
    }
    
    findField("out")->setText(outText);
}

void SyncScreen::displayModeIn()
{
    findField("mode-in")->setText(modeNames[getModeIn()]);
}

void SyncScreen::displayModeOut()
{
    findField("mode-out")->setText(modeNames[getModeOut()]);
}

void SyncScreen::displayReceiveMMC()
{
    auto mmc = std::string(receiveMMCEnabled ? "ON" : "OFF");
    findField("receive-mmc")->setText(mmc);
}

void SyncScreen::displaySendMMC()
{
    auto mmc = std::string(sendMMCEnabled ? "ON" : "OFF");
    findField("send-mmc")->setText(mmc);
}

int SyncScreen::getModeIn()
{
    return modeIn;
}
