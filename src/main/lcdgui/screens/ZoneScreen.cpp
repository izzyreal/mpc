#include "ZoneScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/Layer.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
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
    const bool sound = sampler.lock()->getSound() ? true : false;

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

    ls.lock()->setFunctionKeysArrangement(sound ? 1 : 0);
}

void ZoneScreen::openWindow()
{
    const auto focusedField = getFocusedField();

    if (const auto sound = sampler.lock()->getSound(); !focusedField || !sound)
    {
        return;
    }

    if (const auto focusedFieldName = focusedField->getName();
        focusedFieldName == "snd")
    {
        sampler.lock()->setPreviousScreenName("zone");
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

void ZoneScreen::function(const int f)
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
            sampler.lock()->switchToNextSoundSortType();
            ls.lock()->showPopupForMs(
                "Sorting by " + sampler.lock()->getSoundSortingTypeName(), 200);
            break;
        }
        case 3:
            openScreenById(ScreenId::SndParamsScreen);
            break;
        case 4:
        {
            if (sampler.lock()->getSoundCount() == 0)
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
            sampler.lock()->playX();
            break;
        }
        default:;
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

void ZoneScreen::turnWheel(const int i)
{

    const auto sound = sampler.lock()->getSound();

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

    if (const auto focusedFieldName = focusedField->getName();
        focusedFieldName == "st")
    {
        setZoneStart(selectedZoneIndex,
                     getZoneStart(selectedZoneIndex) + soundInc);
        displaySt();
        displayWave();
    }
    else if (focusedFieldName == "end")
    {
        setZoneEnd(selectedZoneIndex, getZoneEnd(selectedZoneIndex) + soundInc);
        displayEnd();
        displayWave();
    }
    else if (focusedFieldName == "zone")
    {
        setSelectedZoneIndex(selectedZoneIndex + i);
        displayZone();
        displaySt();
        displayEnd();
        displayWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "snd" && i > 0)
    {
        sampler.lock()->selectNextSound();
        initZones();
        displayEnd();
        displaySnd();
        displaySt();
        displayWave();
        displayZone();
    }
    else if (focusedFieldName == "snd" && i < 0)
    {
        sampler.lock()->selectPreviousSound();
        initZones();
        displayEnd();
        displaySnd();
        displaySt();
        displayWave();
        displayZone();
    }
}

void ZoneScreen::setSlider(const int i)
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

    if (const auto focusedFieldName = focusedField->getName();
        focusedFieldName == "st")
    {
        setSliderZoneStart(i);
    }
    else if (focusedFieldName == "end")
    {
        setSliderZoneEnd(i);
    }
}

void ZoneScreen::setSliderZoneStart(const int i)
{
    const auto minZoneStart =
        selectedZoneIndex == 0 ? 0 : zones[selectedZoneIndex - 1][0];
    const auto maxZoneStart = zones[selectedZoneIndex][1];
    const auto rangeLength = maxZoneStart - minZoneStart;
    const auto newZoneStart = i / 124.0 * rangeLength + minZoneStart;
    setZoneStart(selectedZoneIndex, static_cast<int>(newZoneStart));
}

void ZoneScreen::setSliderZoneEnd(const int i)
{
    const auto sound = sampler.lock()->getSound();
    const auto minZoneEnd = zones[selectedZoneIndex][0];
    const auto maxZoneEnd = selectedZoneIndex == zoneCount - 1
                                ? sound->getFrameCount()
                                : zones[selectedZoneIndex + 1][1];
    const auto rangeLength = maxZoneEnd - minZoneEnd;
    const auto newZoneEnd = i / 124.0 * rangeLength + minZoneEnd;
    setZoneEnd(selectedZoneIndex, static_cast<int>(newZoneEnd));
}

void ZoneScreen::displayWave()
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        findWave()->setSampleData(nullptr, true, 0);
        findWave()->setSelection(0, 0);
        return;
    }

    const auto sampleData = sound->getSampleData();
    const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
    findWave()->setSampleData(sampleData, sampler.lock()->getSound()->isMono(),
                              trimScreen->getView());
    findWave()->setSelection(getZoneStart(selectedZoneIndex),
                             getZoneEnd(selectedZoneIndex));
}

void ZoneScreen::displaySnd() const
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        findField("snd")->setText("(no sound)");
        ls.lock()->setFocus("dummy");
        return;
    }

    if (ls.lock()->getFocusedFieldName() == "dummy")
    {
        ls.lock()->setFocus("snd");
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
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void ZoneScreen::displaySt() const
{
    if (sampler.lock()->getSoundCount() != 0)
    {
        findField("st")->setTextPadded(getZoneStart(selectedZoneIndex), " ");
    }
    else
    {
        findField("st")->setText("       0");
    }
}

void ZoneScreen::displayEnd() const
{
    if (sampler.lock()->getSoundCount() != 0)
    {
        findField("end")->setTextPadded(getZoneEnd(selectedZoneIndex), " ");
    }
    else
    {
        findField("end")->setText("       0");
    }
}

void ZoneScreen::displayZone() const
{
    if (sampler.lock()->getSoundCount() == 0)
    {
        findField("zone")->setTextPadded(1);
        return;
    }

    findField("zone")->setTextPadded(selectedZoneIndex + 1);
}

void ZoneScreen::initZones()
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        zones.clear();
        selectedZoneIndex = 0;
        return;
    }

    if (numberOfFramesAtLastZoneInitialization == sound->getFrameCount() &&
        zones.size() == zoneCount)
    {
        return;
    }

    zones.clear();

    const float zoneLength =
        sound->getFrameCount() / static_cast<float>(zoneCount);
    float zoneStart = 0.f;

    for (int i = 0; i < zoneCount - 1; i++)
    {
        zones.emplace_back(2);
        zones[i][0] = static_cast<int>(floor(zoneStart));
        zones[i][1] = static_cast<int>(floor(zoneStart + zoneLength));
        zoneStart += zoneLength;
    }

    zones.emplace_back(2);
    zones[zoneCount - 1][0] = static_cast<int>(floor(zoneStart));
    zones[zoneCount - 1][1] = sound->getFrameCount();
    selectedZoneIndex = 0;

    numberOfFramesAtLastZoneInitialization = sound->getFrameCount();
}

void ZoneScreen::setZoneStart(const int zoneIndex, int start)
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

int ZoneScreen::getZoneStart(const int zoneIndex) const
{
    if (zoneIndex >= zones.size())
    {
        return 0;
    }

    return zones[zoneIndex][0];
}

void ZoneScreen::setZoneEnd(const int zoneIndex, int end)
{
    const auto length = sampler.lock()->getSound()->getFrameCount();

    if (end < zones[zoneIndex][0])
    {
        end = zones[zoneIndex][0];
    }

    if (zoneIndex < zoneCount - 1 && end > zones[zoneIndex + 1][1])
    {
        end = zones[zoneIndex + 1][1];
    }

    if (zoneIndex == zoneCount - 1 && end > length)
    {
        end = length;
    }

    zones[zoneIndex][1] = end;

    if (zoneIndex != zoneCount - 1)
    {
        zones[zoneIndex + 1][0] = end;
    }

    displayEnd();
    displayWave();
}

int ZoneScreen::getZoneEnd(const int zoneIndex) const
{
    if (zoneIndex >= zones.size())
    {
        return 0;
    }

    return zones[zoneIndex][1];
}
int ZoneScreen::getZoneCount() const
{
    return zoneCount;
}

void ZoneScreen::setZoneCount(const int zoneCountToUse)
{
    zoneCount = zoneCountToUse;
}

int ZoneScreen::getSelectedZoneIndex() const
{
    return selectedZoneIndex;
}

void ZoneScreen::setSelectedZoneIndex(const int zoneIndexToUse)
{
    selectedZoneIndex =
        std::clamp(zoneIndexToUse, 0, std::max(0, zoneCount - 1));

    displayWave();
    displaySt();
    displayEnd();
    displayZone();
}

std::vector<int> ZoneScreen::getZone() const
{
    return std::vector{getZoneStart(selectedZoneIndex),
                       getZoneEnd(selectedZoneIndex)};
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
    auto sound = sampler.lock()->getSound();

    if (candidate != INT_MAX)
    {
        if (const auto focusedFieldName = focusedField->getName();
            focusedFieldName == "st")
        {
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            zoneScreen->setZoneStart(zoneScreen->selectedZoneIndex, candidate);
            displaySt();
            displayWave();
        }
        else if (focusedFieldName == "end")
        {
            const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
            zoneScreen->setZoneEnd(zoneScreen->selectedZoneIndex, candidate);
            displayEnd();
            displayWave();
        }
    }
}
