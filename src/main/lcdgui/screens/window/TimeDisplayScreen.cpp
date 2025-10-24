#include "TimeDisplayScreen.hpp"

using namespace mpc::lcdgui::screens::window;

TimeDisplayScreen::TimeDisplayScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "time-display", layerIndex)
{
}

void TimeDisplayScreen::open()
{
    displayDisplayStyle();
    displayStartTime();
    displayFrameRate();
}

void TimeDisplayScreen::turnWheel(int i)
{

    const auto startTime = sequencer.lock()->getActiveSequence()->getStartTime();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "displaystyle")
    {
        setDisplayStyle(displayStyle + i);
    }
    else if (focusedFieldName == "hours")
    {
        setHours(startTime.hours + i);
    }
    else if (focusedFieldName == "minutes")
    {
        setMinutes(startTime.minutes + i);
    }
    else if (focusedFieldName == "seconds")
    {
        setSeconds(startTime.seconds + i);
    }
    else if (focusedFieldName == "frames")
    {
        setFrames(startTime.frames + i);
    }
    else if (focusedFieldName == "frame-decimals")
    {
        setFrameDecimals(startTime.frameDecimals + i);
    }
    else if (focusedFieldName == "framerate")
    {
        setFrameRate(frameRate + i);
    }
}

void TimeDisplayScreen::displayDisplayStyle()
{
    findField("displaystyle")->setText(displayStyleNames[displayStyle]);
}

void TimeDisplayScreen::displayStartTime()
{
    const auto startTime = sequencer.lock()->getActiveSequence()->getStartTime();
    findField("hours")->setText(StrUtil::padLeft(std::to_string(startTime.hours), "0", 2));
    findField("minutes")->setText(StrUtil::padLeft(std::to_string(startTime.minutes), "0", 2));
    findField("seconds")->setText(StrUtil::padLeft(std::to_string(startTime.seconds), "0", 2));
    findField("frames")->setText(StrUtil::padLeft(std::to_string(startTime.frames), "0", 2));
    findField("frame-decimals")->setText(StrUtil::padLeft(std::to_string(startTime.frameDecimals), "0", 2));
}

void TimeDisplayScreen::displayFrameRate()
{
    findField("framerate")->setText(frameRateNames[frameRate]);
}

void TimeDisplayScreen::setDisplayStyle(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }
    displayStyle = i;
    displayDisplayStyle();
}

void TimeDisplayScreen::setHours(int i)
{
    sequencer.lock()->getActiveSequence()->getStartTime().hours = std::clamp<uint8_t>(i, 0, 23);
    displayStartTime();
}

void TimeDisplayScreen::setMinutes(int i)
{
    sequencer.lock()->getActiveSequence()->getStartTime().minutes = std::clamp<uint8_t>(i, 0, 59);
    displayStartTime();
}

void TimeDisplayScreen::setSeconds(int i)
{
    sequencer.lock()->getActiveSequence()->getStartTime().seconds = std::clamp<uint8_t>(i, 0, 59);
    displayStartTime();
}

void TimeDisplayScreen::setFrames(int i)
{
    sequencer.lock()->getActiveSequence()->getStartTime().frames = std::clamp<uint8_t>(i, 0, 29);
    displayStartTime();
}

void TimeDisplayScreen::setFrameDecimals(int i)
{
    sequencer.lock()->getActiveSequence()->getStartTime().frameDecimals = std::clamp<uint8_t>(i, 0, 99);
    displayStartTime();
}

void TimeDisplayScreen::setFrameRate(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }

    frameRate = i;
    displayFrameRate();
}

int TimeDisplayScreen::getDisplayStyle()
{
    return displayStyle;
}
