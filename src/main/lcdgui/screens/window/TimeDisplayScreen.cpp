#include "TimeDisplayScreen.hpp"

#include "StrUtil.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;

TimeDisplayScreen::TimeDisplayScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "time-display", layerIndex)
{
}

void TimeDisplayScreen::open()
{
    displayDisplayStyle();
    displayStartTime();
    displayFrameRate();
}

void TimeDisplayScreen::turnWheel(const int i)
{
    const auto startTime = sequencer->getActiveSequence()->getStartTime();

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

void TimeDisplayScreen::displayDisplayStyle() const
{
    findField("displaystyle")->setText(displayStyleNames[displayStyle]);
}

void TimeDisplayScreen::displayStartTime() const
{
    const auto startTime = sequencer->getActiveSequence()->getStartTime();
    findField("hours")->setText(
        StrUtil::padLeft(std::to_string(startTime.hours), "0", 2));
    findField("minutes")->setText(
        StrUtil::padLeft(std::to_string(startTime.minutes), "0", 2));
    findField("seconds")->setText(
        StrUtil::padLeft(std::to_string(startTime.seconds), "0", 2));
    findField("frames")->setText(
        StrUtil::padLeft(std::to_string(startTime.frames), "0", 2));
    findField("frame-decimals")
        ->setText(
            StrUtil::padLeft(std::to_string(startTime.frameDecimals), "0", 2));
}

void TimeDisplayScreen::displayFrameRate() const
{
    findField("framerate")->setText(frameRateNames[frameRate]);
}

void TimeDisplayScreen::setDisplayStyle(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }
    displayStyle = i;
    displayDisplayStyle();
}

void TimeDisplayScreen::setHours(const int i) const
{
    sequencer->getActiveSequence()->getStartTime().hours =
        std::clamp<uint8_t>(i, 0, 23);
    displayStartTime();
}

void TimeDisplayScreen::setMinutes(const int i) const
{
    sequencer->getActiveSequence()->getStartTime().minutes =
        std::clamp<uint8_t>(i, 0, 59);
    displayStartTime();
}

void TimeDisplayScreen::setSeconds(const int i) const
{
    sequencer->getActiveSequence()->getStartTime().seconds =
        std::clamp<uint8_t>(i, 0, 59);
    displayStartTime();
}

void TimeDisplayScreen::setFrames(const int i) const
{
    sequencer->getActiveSequence()->getStartTime().frames =
        std::clamp<uint8_t>(i, 0, 29);
    displayStartTime();
}

void TimeDisplayScreen::setFrameDecimals(const int i) const
{
    sequencer->getActiveSequence()->getStartTime().frameDecimals =
        std::clamp<uint8_t>(i, 0, 99);
    displayStartTime();
}

void TimeDisplayScreen::setFrameRate(const int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }

    frameRate = i;
    displayFrameRate();
}

int TimeDisplayScreen::getDisplayStyle() const
{
    return displayStyle;
}
