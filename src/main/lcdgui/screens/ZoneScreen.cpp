#include "ZoneScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/Layer.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "lcdgui/screens/window/NumberOfZonesScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "sampler/Sampler.hpp"

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

ZoneScreen::ZoneScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "zone", layerIndex)
{
    addChildT<Wave>()->setFine(false);
}

void ZoneScreen::open()
{
    const bool sound = sampler->getSound() ? true : false;

    initZones();

    findField("snd")->setFocusable(sound);
    findField("playx")->setFocusable(sound);
    findField("st")->setFocusable(sound);
    findField("st")->enableTwoDots();
    findField("end")->setFocusable(sound);
    findField("end")->enableTwoDots();
    findField("zone")->setFocusable(sound);
    findField("dummy")->setFocusable(!sound);

    displayWave();
    displaySnd();
    displayPlayX();
    displaySt();
    displayEnd();
    displayZone();

    ls->setFunctionKeysArrangement(sound ? 1 : 0);
}

void ZoneScreen::openWindow()
{

    const auto focusedField = getFocusedField();

    const auto sound = sampler->getSound();

    if (!focusedField || !sound)
    {
        return;
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "snd")
    {
        sampler->setPreviousScreenName("zone");
        openScreenById(ScreenId::SoundScreen);
    }
    else if (focusedFieldName == "zone")
    {
        openScreenById(ScreenId::NumberOfZonesScreen);
    }
    else if (focusedFieldName == "st")
    {
        openScreenById(ScreenId::ZoneStartFineScreen);
    }
    else if (focusedFieldName == "end")
    {
        openScreenById(ScreenId::ZoneEndFineScreen);
    }
}

void ZoneScreen::function(int f)
{

    switch (f)
    {
        case 0:
            openScreenById(ScreenId::TrimScreen);
            break;
        case 1:
            openScreenById(ScreenId::LoopScreen);
            break;
        case 2:
        {
            sampler->switchToNextSoundSortType();
            ls->showPopupForMs(
                "Sorting by " + sampler->getSoundSortingTypeName(), 200);
            break;
        }
        case 3:
            openScreenById(ScreenId::SndParamsScreen);
            break;
        case 4:
        {
            if (sampler->getSoundCount() == 0)
            {
                return;
            }

            const auto editSoundScreen =
                mpc.screens->get<ScreenId::EditSoundScreen>();
            editSoundScreen->setReturnToScreenName("zone");
            openScreenById(ScreenId::EditSoundScreen);
            break;
        }
        case 5:
        {
            sampler->playX();
            break;
        }
    }
}

void ZoneScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void ZoneScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void ZoneScreen::turnWheel(int i)
{

    const auto sound = sampler->getSound();

    const auto focusedField = getFocusedField();

    if (!focusedField || !sound)
    {
        return;
    }

    auto soundInc = getSoundIncrement(i);

    if (focusedField->isSplit())
    {
        soundInc = focusedField->getSplitIncrement(i >= 0);
    }

    if (focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "st")
    {
        setZoneStart(zone, getZoneStart(zone) + soundInc);
        displaySt();
        displayWave();
    }
    else if (focusedFieldName == "end")
    {
        setZoneEnd(zone, getZoneEnd(zone) + soundInc);
        displayEnd();
        displayWave();
    }
    else if (focusedFieldName == "zone")
    {
        setZone(zone + i);
        displayZone();
        displaySt();
        displayEnd();
        displayWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler->setPlayX(sampler->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "snd" && i > 0)
    {
        sampler->selectNextSound();
        initZones();
        displayEnd();
        displaySnd();
        displaySt();
        displayWave();
        displayZone();
    }
    else if (focusedFieldName == "snd" && i < 0)
    {
        sampler->selectPreviousSound();
        initZones();
        displayEnd();
        displaySnd();
        displaySt();
        displayWave();
        displayZone();
    }
}

void ZoneScreen::setSlider(int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    const auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "st")
    {
        setSliderZoneStart(i);
    }
    else if (focusedFieldName == "end")
    {
        setSliderZoneEnd(i);
    }
}

void ZoneScreen::setSliderZoneStart(int i)
{
    const auto minZoneStart = zone == 0 ? 0 : zones[zone - 1][0];
    const auto maxZoneStart = zones[zone][1];
    const auto rangeLength = maxZoneStart - minZoneStart;
    const auto newZoneStart = i / 124.0 * rangeLength + minZoneStart;
    setZoneStart(zone, static_cast<int>(newZoneStart));
}

void ZoneScreen::setSliderZoneEnd(int i)
{
    const auto sound = sampler->getSound();
    const auto minZoneEnd = zones[zone][0];
    const auto maxZoneEnd =
        zone == numberOfZones - 1 ? sound->getFrameCount() : zones[zone + 1][1];
    const auto rangeLength = maxZoneEnd - minZoneEnd;
    const auto newZoneEnd = i / 124.0 * rangeLength + minZoneEnd;
    setZoneEnd(zone, static_cast<int>(newZoneEnd));
}

void ZoneScreen::displayWave()
{
    const auto sound = sampler->getSound();

    if (!sound)
    {
        findWave()->setSampleData(nullptr, true, 0);
        findWave()->setSelection(0, 0);
        return;
    }

    const auto sampleData = sound->getSampleData();
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
    findWave()->setSampleData(sampleData, sampler->getSound()->isMono(),
                              trimScreen->view);
    findWave()->setSelection(getZoneStart(zone), getZoneEnd(zone));
}

void ZoneScreen::displaySnd() const
{
    const auto sound = sampler->getSound();

    if (!sound)
    {
        findField("snd")->setText("(no sound)");
        ls->setFocus("dummy");
        return;
    }

    if (ls->getFocusedFieldName() == "dummy")
    {
        ls->setFocus("snd");
    }

    auto sampleName = sound->getName();

    if (!sound->isMono())
    {
        sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";
    }

    findField("snd")->setText(sampleName);
}

void ZoneScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void ZoneScreen::displaySt() const
{
    if (sampler->getSoundCount() != 0)
    {
        findField("st")->setTextPadded(getZoneStart(zone), " ");
    }
    else
    {
        findField("st")->setText("       0");
    }
}

void ZoneScreen::displayEnd() const
{
    if (sampler->getSoundCount() != 0)
    {
        findField("end")->setTextPadded(getZoneEnd(zone), " ");
    }
    else
    {
        findField("end")->setText("       0");
    }
}

void ZoneScreen::displayZone() const
{
    if (sampler->getSoundCount() == 0)
    {
        findField("zone")->setTextPadded(1);
        return;
    }

    findField("zone")->setTextPadded(zone + 1);
}

void ZoneScreen::initZones()
{
    const auto sound = sampler->getSound();

    if (!sound)
    {
        zones.clear();
        zone = 0;
        return;
    }

    if (numberOfFramesAtLastZoneInitialization == sound->getFrameCount() &&
        zones.size() == numberOfZones)
    {
        return;
    }

    zones.clear();

    const float zoneLength = sound->getFrameCount() / float(numberOfZones);
    float zoneStart = 0.f;

    for (int i = 0; i < numberOfZones - 1; i++)
    {
        zones.emplace_back(2);
        zones[i][0] = (int)floor(zoneStart);
        zones[i][1] = (int)floor(zoneStart + zoneLength);
        zoneStart += zoneLength;
    }

    zones.emplace_back(2);
    zones[numberOfZones - 1][0] = (int)floor(zoneStart);
    zones[numberOfZones - 1][1] = sound->getFrameCount();
    zone = 0;

    numberOfFramesAtLastZoneInitialization = sound->getFrameCount();
}

void ZoneScreen::setZoneStart(int zoneIndex, int start)
{
    if (start > zones[zoneIndex][1])
    {
        start = zones[zoneIndex][1];
    }

    if (zoneIndex == 0 && start < 0)
    {
        start = 0;
    }

    if (zoneIndex > 0 && start < zones[zoneIndex - 1][0])
    {
        start = zones[zoneIndex - 1][0];
    }

    zones[zoneIndex][0] = start;

    if (zoneIndex != 0)
    {
        zones[zoneIndex - 1][1] = start;
    }

    displaySt();
    displayWave();
}

int ZoneScreen::getZoneStart(int zoneIndex) const
{
    if (zoneIndex >= zones.size())
    {
        return 0;
    }

    return zones[zoneIndex][0];
}

void ZoneScreen::setZoneEnd(int zoneIndex, int end)
{
    const auto length = sampler->getSound()->getFrameCount();

    if (end < zones[zoneIndex][0])
    {
        end = zones[zoneIndex][0];
    }

    if (zoneIndex < numberOfZones - 1 && end > zones[zoneIndex + 1][1])
    {
        end = zones[zoneIndex + 1][1];
    }

    if (zoneIndex == numberOfZones - 1 && end > length)
    {
        end = length;
    }

    zones[zoneIndex][1] = end;

    if (zoneIndex != numberOfZones - 1)
    {
        zones[zoneIndex + 1][0] = end;
    }

    displayEnd();
    displayWave();
}

int ZoneScreen::getZoneEnd(int zoneIndex) const
{
    if (zoneIndex >= zones.size())
    {
        return 0;
    }

    return zones[zoneIndex][1];
}

void ZoneScreen::setZone(int i)
{
    if (i < 0 || i >= numberOfZones)
    {
        return;
    }

    zone = i;

    displayWave();
    displaySt();
    displayEnd();
    displayZone();
}

std::vector<int> ZoneScreen::getZone() const
{
    return std::vector{getZoneStart(zone), getZoneEnd(zone)};
}

void ZoneScreen::pressEnter()
{
    if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        openScreenById(ScreenId::SaveScreen);
        return;
    }

    const auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }

    if (!focusedField->isTypeModeEnabled())
    {
        return;
    }

    const auto candidate = focusedField->enter();
    auto sound = sampler->getSound();

    if (candidate != INT_MAX)
    {
        const auto focusedFieldName = focusedField->getName();

        if (focusedFieldName == "st")
        {
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            zoneScreen->setZoneStart(zoneScreen->zone, candidate);
            displaySt();
            displayWave();
        }
        else if (focusedFieldName == "end")
        {
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            zoneScreen->setZoneEnd(zoneScreen->zone, candidate);
            displayEnd();
            displayWave();
        }
    }
}
