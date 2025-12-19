#include "SndParamsScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "lcdgui/screens/ZoneScreen.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

SndParamsScreen::SndParamsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "params", layerIndex)
{
}

void SndParamsScreen::open()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    zoneScreen->initZones();

    const bool soundIsLoaded = sampler.lock()->getSound() != nullptr;

    findField("snd")->setFocusable(soundIsLoaded);
    findField("playx")->setFocusable(soundIsLoaded);
    findField("level")->setFocusable(soundIsLoaded);
    findField("tune")->setFocusable(soundIsLoaded);
    findField("beat")->setFocusable(soundIsLoaded);
    findField("dummy")->setFocusable(!soundIsLoaded);

    displaySnd();
    displayPlayX();
    displayLevel();
    displayTune();
    displayBeat();
    displaySampleAndNewTempo();

    ls.lock()->setFunctionKeysArrangement(soundIsLoaded ? 1 : 0);
}

void SndParamsScreen::openWindow()
{

    const auto focusedField = getFocusedField();

    const auto sound = sampler.lock()->getSound();

    if (!focusedField || !sound)
    {
        return;
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "snd")
    {
        sampler.lock()->setPreviousScreenName("params");
        openScreenById(ScreenId::SoundScreen);
    }
}

void SndParamsScreen::function(int f)
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
            openScreenById(ScreenId::ZoneScreen);
            break;
        case 3:
        {
            sampler.lock()->switchToNextSoundSortType();
            ls.lock()->showPopupForMs(
                "Sorting by " + sampler.lock()->getSoundSortingTypeName(), 200);
            break;
        }
        case 4:
        {
            if (sampler.lock()->getSoundCount() == 0)
            {
                return;
            }

            const auto editSoundScreen =
                mpc.screens->get<ScreenId::EditSoundScreen>();
            editSoundScreen->setReturnToScreenName("trim");
            openScreenById(ScreenId::EditSoundScreen);
            break;
        }
        case 5:
        {
            sampler.lock()->playX();
            break;
        }
    }
}

void SndParamsScreen::turnWheel(int i)
{
    const auto sound = sampler.lock()->getSound();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "snd" && i > 0)
    {
        sampler.lock()->selectNextSound();
        displayBeat();
        displayLevel();
        displaySampleAndNewTempo();
        displaySnd();
        displayTune();
    }
    else if (focusedFieldName == "snd" && i < 0)
    {
        sampler.lock()->selectPreviousSound();
        displayBeat();
        displayLevel();
        displaySampleAndNewTempo();
        displaySnd();
        displayTune();
    }
    else if (focusedFieldName == "level")
    {
        sound->setLevel(sound->getSndLevel() + i);
        displayLevel();
    }
    else if (focusedFieldName == "tune")
    {
        sound->setTune(sound->getTune() + i);
        displayTune();
        displaySampleAndNewTempo();
    }
    else if (focusedFieldName == "beat")
    {
        sound->setBeatCount(sound->getBeatCount() + i);
        displayBeat();
        displaySampleAndNewTempo();
    }
}

void SndParamsScreen::displayLevel() const
{
    const auto sound = sampler.lock()->getSound();

    if (sound)
    {
        findField("level")->setText(std::to_string(sound->getSndLevel()));
    }
    else
    {
        findField("level")->setText("100");
    }
}

void SndParamsScreen::displayTune() const
{
    const auto sound = sampler.lock()->getSound();

    if (sound)
    {
        findField("tune")->setText(std::to_string(sound->getTune()));
    }
    else
    {
        findField("tune")->setText("0");
    }
}

void SndParamsScreen::displayBeat() const
{
    const auto sound = sampler.lock()->getSound();

    if (sound)
    {
        findField("beat")->setText(std::to_string(sound->getBeatCount()));
    }
    else
    {
        findField("beat")->setText("4");
    }
}

void SndParamsScreen::displaySampleAndNewTempo() const
{
    auto sound = sampler.lock()->getSound();

    if (!sound || !sound->isLoopEnabled())
    {
        findLabel("sample-tempo")->setText("");
        findLabel("new-tempo")->setText("");
        return;
    }

    auto length = sound->getEnd() - sound->getLoopTo();
    auto lengthMs = (float)(length / (sound->getSampleRate() * 0.001));

    auto bpm = (int)(600000.0 / (lengthMs / sound->getBeatCount()));
    auto bpmString = std::to_string(bpm);
    auto part1 = bpmString.substr(0, bpmString.length() - 1);
    auto part2 = bpmString.substr(bpmString.length() - 1);

    if (bpm < 300 || bpm > 9999)
    {
        part1 = "---";
        part2 = "-";
    }

    bpmString = part1 + "." + part2;
    bpmString = StrUtil::padLeft(bpmString, " ", 5);

    bpmString = Util::replaceDotWithSmallSpaceDot(bpmString);

    findLabel("sample-tempo")->setText("Sample tempo=" + bpmString);

    auto newBpm = (int)(pow(2.0, sound->getTune() / 120.0) * bpm);
    bpmString = std::to_string(newBpm);
    part1 = bpmString.substr(0, bpmString.length() - 1);
    part2 = bpmString.substr(bpmString.length() - 1);

    if (newBpm < 300 || newBpm > 9999)
    {
        part1 = "---";
        part2 = "-";
    }

    bpmString = part1 + "." + part2;
    bpmString = StrUtil::padLeft(bpmString, " ", 5);

    bpmString = Util::replaceDotWithSmallSpaceDot(bpmString);

    findLabel("new-tempo")->setText("New tempo=" + bpmString);
}

void SndParamsScreen::displaySnd() const
{
    const auto sound = sampler.lock()->getSound();

    if (sampler.lock()->getSoundCount() == 0)
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

void SndParamsScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}
