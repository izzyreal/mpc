#include "LocateScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

LocateScreen::LocateScreen(mpc::Mpc &mpc, const int layerIndex)
        : ScreenComponent(mpc, "locate", layerIndex)
{
}

void LocateScreen::function(int i)
{
    init();

    switch (i)
    {
        case 1:
        {
            if (param.length() != 1)
            {
                break;
            }

            const auto locationIndex = std::stoi(param) - 1;
            auto& location = locations[locationIndex];
            std::get<0>(location) = barIndex;
            std::get<1>(location) = beatIndex;
            std::get<2>(location) = clock;

            openScreen("sequencer");
            break;
        }
        case 3:
            openScreen("sequencer");
            break;
        case 4:
            if (param.length() == 1)
            {
                const auto locationIndex = std::stoi(param) - 1;
                auto& location = locations[locationIndex];

                const auto clampedBarIndex =
                        std::clamp<uint16_t>(std::get<0>(location), 0, getMaxBarIndexForThisSequence());

                const auto clampedBeatIndex =
                        std::clamp<uint8_t>(std::get<1>(location), 0, getMaxBeatIndexForThisBar());

                const auto clampedClock =
                        std::clamp<uint8_t>(std::get<2>(location), 0, getMaxClockForThisBar());

                sequencer.lock()->setBar(clampedBarIndex);
                sequencer.lock()->setBeat(clampedBeatIndex);
                sequencer.lock()->setClock(clampedClock);
            }
            else
            {
                sequencer.lock()->setBar(barIndex);
                sequencer.lock()->setBeat(beatIndex);
                sequencer.lock()->setClock(clock);
            }
            openScreen("sequencer");
            break;
        default:
            break;
    }
}

void LocateScreen::turnWheel(int i)
{
    init();

    if (param == "goto0")
    {
        setBarIndex(barIndex + i);
    }
    else if (param == "goto1")
    {
        setBeatIndex(beatIndex + i);
    }
    else if (param == "goto2")
    {
        setClock(clock + i);
    }
}

void LocateScreen::open()
{
    barIndex = sequencer.lock()->getCurrentBarIndex();
    beatIndex = sequencer.lock()->getCurrentBeatIndex();
    clock = sequencer.lock()->getCurrentClockNumber();

    displayBar();
    displayBeat();
    displayClock();
    displayLocations();
}

void LocateScreen::displayBar()
{
    findField("goto0")->setText(StrUtil::padLeft(std::to_string(barIndex + 1), "0", 3));
}

void LocateScreen::displayBeat()
{
    findField("goto1")->setText(StrUtil::padLeft(std::to_string(beatIndex + 1), "0", 2));
}

void LocateScreen::displayClock()
{
    findField("goto2")->setText(StrUtil::padLeft(std::to_string(clock), "0", 2));
}

void LocateScreen::displayLocations()
{
    for (int i = 0; i < 9; i++)
    {
        const auto location = locations[i];

        const auto locationBarIndex = std::get<0>(location);
        const auto locationBeatIndex = std::get<1>(location);
        const auto locationClock = std::get<2>(location);

        const auto locationBarStr = StrUtil::padLeft(std::to_string(locationBarIndex + 1), "0", 3);
        const auto locationBeatStr = StrUtil::padLeft(std::to_string(locationBeatIndex + 1), "0", 2);
        const auto locationClockStr = StrUtil::padLeft(std::to_string(locationClock), "0", 2);

        const auto locationStr = locationBarStr + "." + locationBeatStr + "." + locationClockStr;

        findField(std::to_string(i+1))->setText(locationStr);
    }
}

void LocateScreen::setBarIndex(int16_t newBarIndex)
{
    const int maxBarIndex = getMaxBarIndexForThisSequence();
    barIndex = std::clamp<int16_t>(newBarIndex, 0, maxBarIndex);
    displayBar();

    if (barIndex == maxBarIndex)
    {
        beatIndex = 0;
        displayBeat();

        clock = 0;
        displayClock();
    }

    const int maxBeatIndex = getMaxBeatIndexForThisBar();

    if (beatIndex > maxBeatIndex)
    {
        setBeatIndex(maxBeatIndex);
    }

    const int maxClock = getMaxClockForThisBar();

    if (clock > maxClock)
    {
        setClock(maxClock);
    }
}

void LocateScreen::setBeatIndex(int8_t newBeatIndex)
{
    if (barIndex == getMaxBarIndexForThisSequence())
    {
        return;
    }

    const int maxBeatIndex = getMaxBeatIndexForThisBar();
    beatIndex = std::clamp<int16_t>(newBeatIndex, 0, maxBeatIndex);

    displayBeat();

    if (beatIndex == maxBeatIndex)
    {
        clock = 0;
        displayClock();
    }
}

void LocateScreen::setClock(int8_t newClock)
{
    if (barIndex == getMaxBarIndexForThisSequence())
    {
        return;
    }

    clock = std::clamp<int8_t>(newClock, 0, getMaxClockForThisBar());
    displayClock();
}

uint16_t LocateScreen::getMaxBarIndexForThisSequence()
{
    return std::clamp<uint16_t>(sequencer.lock()->getActiveSequence()->getLastBarIndex() + 1, 0, 998);
}

uint8_t LocateScreen::getMaxBeatIndexForThisBar()
{
    return sequencer.lock()->getActiveSequence()->getNumerator(barIndex) - 1;
}

uint8_t LocateScreen::getMaxClockForThisBar()
{
    return 96 * (4.0 / sequencer.lock()->getActiveSequence()->getDenominator(barIndex)) - 1;
}

void LocateScreen::setLocations(const std::vector<Location>& newLocations)
{
    assert(newLocations.size() == 9);
    locations = newLocations;
}

std::vector<LocateScreen::Location> &LocateScreen::getLocations()
{
    return locations;
}
