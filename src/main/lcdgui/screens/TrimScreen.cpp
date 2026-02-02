#include "TrimScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoopScreen.hpp"
#include "lcdgui/screens/ZoneScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "lcdgui/Layer.hpp"
#include "sampler/Sampler.hpp"

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

TrimScreen::TrimScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "trim", layerIndex)
{
    addChildT<Wave>()->setFine(false);
}

void TrimScreen::open()
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    zoneScreen->initZones();

    findField("view")->setAlignment(Alignment::Centered);
    const bool soundIsLoaded = sampler.lock()->getSound() != nullptr;

    findField("snd")->setFocusable(soundIsLoaded);
    findField("playx")->setFocusable(soundIsLoaded);
    findField("st")->setFocusable(soundIsLoaded);
    findField("st")->enableTwoDots();
    findField("end")->setFocusable(soundIsLoaded);
    findField("end")->enableTwoDots();
    findField("view")->setFocusable(soundIsLoaded);
    findField("dummy")->setFocusable(!soundIsLoaded);

    displaySnd();
    displayPlayX();
    displaySt();
    displayEnd();
    displayView();
    displayWave();

    ls.lock()->setFunctionKeysArrangement(soundIsLoaded ? 1 : 0);
}

void TrimScreen::openWindow()
{

    const auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }

    if (const auto focusedFieldName = focusedField->getName();
        focusedFieldName == "snd")
    {
        sampler.lock()->setPreviousScreenName("trim");
        openScreenById(ScreenId::SoundScreen);
    }
    else if (focusedFieldName == "st")
    {
        openScreenById(ScreenId::StartFineScreen);
    }
    else if (focusedFieldName == "end")
    {
        openScreenById(ScreenId::EndFineScreen);
    }
}

void TrimScreen::function(const int f)
{
    switch (f)
    {
        case 0:
        {
            sampler.lock()->switchToNextSoundSortType();
            ls.lock()->showPopupForMs(
                "Sorting by " + sampler.lock()->getSoundSortingTypeName(), 200);
            break;
        }
        case 1:
            openScreenById(ScreenId::LoopScreen);
            break;
        case 2:
            openScreenById(ScreenId::ZoneScreen);
            break;
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
            editSoundScreen->setReturnToScreenName("trim");

            openScreenById(ScreenId::EditSoundScreen);
            break;
        }
        case 5:
            sampler.lock()->playX();
            break;
        default:;
    }
}

void TrimScreen::turnWheel(const int i)
{

    const auto sound = sampler.lock()->getSound();

    const auto focusedField = getFocusedField();

    if (!focusedField || !sound)
    {
        return;
    }

    auto const oldLength = sound->getEnd() - sound->getStart();

    auto soundInc = getSoundIncrement(i);

    if (focusedField->isSplit())
    {
        soundInc = focusedField->getSplitIncrement(i >= 0);
    }

    if (focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
    }

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "st")
    {
        if (smplLngthFix &&
            sound->getStart() + soundInc + oldLength > sound->getFrameCount())
        {
            return;
        }

        sound->setStart(sound->getStart() + soundInc);

        displaySt();

        if (sound->getEnd() == sound->getStart())
        {
            displayEnd();
        }

        if (smplLngthFix)
        {
            sound->setEnd(sound->getStart() + oldLength);
            displayEnd();
        }

        displayWave();
    }
    else if (focusedFieldName == "end")
    {
        if (smplLngthFix && sound->getEnd() + soundInc - oldLength < 0)
        {
            return;
        }

        sound->setEnd(sound->getEnd() + soundInc);

        displayEnd();

        if (sound->getEnd() == sound->getStart())
        {
            displaySt();
        }

        if (smplLngthFix)
        {
            sound->setStart(sound->getEnd() - oldLength);
            displaySt();
        }

        displayWave();
    }
    else if (focusedFieldName == "view")
    {
        setView(view + i);
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "snd" && i > 0)
    {
        sampler.lock()->selectNextSound();
        displaySnd();
        displayEnd();
        displayPlayX();
        displaySt();
        displayView();
        displayWave();
    }
    else if (focusedFieldName == "snd" && i < 0)
    {
        sampler.lock()->selectPreviousSound();
        displaySnd();
        displayEnd();
        displayPlayX();
        displaySt();
        displayView();
        displayWave();
    }
}

void TrimScreen::setSlider(const int i)
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
        setSliderStart(i);
        displayWave();
    }
    else if (focusedFieldName == "end")
    {
        setSliderEnd(i);
        displayWave();
    }
}

void TrimScreen::setSliderStart(const int i) const
{
    const auto sound = sampler.lock()->getSound();
    auto const oldLength = sound->getEnd() - sound->getStart();
    auto candidatePos = static_cast<int>(i / 124.0 * sound->getFrameCount());

    const auto maxPos = smplLngthFix ? sound->getFrameCount() - oldLength
                                     : sound->getFrameCount();

    if (candidatePos > maxPos)
    {
        candidatePos = maxPos;
    }

    sound->setStart(candidatePos);
    displaySt();

    if (sound->getEnd() == sound->getStart())
    {
        displayEnd();
    }

    if (smplLngthFix)
    {
        sound->setEnd(sound->getStart() + oldLength);
        displayEnd();
    }
}

void TrimScreen::setSliderEnd(const int i) const
{
    const auto sound = sampler.lock()->getSound();
    const auto newValue = static_cast<int>(i / 124.0 * sound->getFrameCount());
    setEnd(newValue);
    displayEnd();
}

void TrimScreen::setEnd(int newValue) const
{
    const auto loopLengthIsFixed =
        mpc.screens->get<ScreenId::LoopScreen>()->isLoopLengthFixed();
    const auto sound = sampler.lock()->getSound();

    const auto oldSoundLength = sound->getEnd() - sound->getStart();
    const auto oldLoopLength = sound->getEnd() - sound->getLoopTo();

    auto lowerBound = 0;
    const auto upperBound = sound->getFrameCount();

    const int oldStart = sound->getStart();

    if (smplLngthFix)
    {
        lowerBound = oldSoundLength;
    }

    if (loopLengthIsFixed)
    {
        lowerBound = std::max<int>(lowerBound, oldLoopLength);
    }

    if (newValue < lowerBound)
    {
        newValue = lowerBound;
    }
    if (newValue > upperBound)
    {
        newValue = upperBound;
    }

    sound->setEnd(newValue);

    if (loopLengthIsFixed)
    {
        sound->setLoopTo(sound->getEnd() - oldLoopLength);
    }

    if (smplLngthFix)
    {
        sound->setStart(sound->getEnd() - oldSoundLength);
    }

    if (oldStart != sound->getStart())
    {
        displaySt();
    }
}

int TrimScreen::getView() const
{
    return view;
}

bool TrimScreen::isSampleLengthFixed() const
{
    return smplLngthFix;
}

void TrimScreen::setSampleLengthFixed(const bool b)
{
    smplLngthFix = b;
}

void TrimScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void TrimScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

// Can be called from another layer, i.e. Start Fine and End Fine windows
void TrimScreen::pressEnter()
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

    auto candidate = focusedField->enter();
    const auto sound = sampler.lock()->getSound();
    auto const oldLength = sound->getEnd() - sound->getStart();

    if (candidate != INT_MAX)
    {
        if (const auto focusedFieldName = focusedField->getName();
            focusedFieldName == "st" || focusedFieldName == "start")
        {
            if (smplLngthFix && candidate + oldLength > sound->getFrameCount())
            {
                candidate = sound->getFrameCount() - oldLength;
            }

            sound->setStart(candidate);

            if (smplLngthFix)
            {
                sound->setEnd(sound->getStart() + oldLength);
            }

            displaySt();
            displayEnd();
            displayWave();
        }
        else if (focusedFieldName == "end")
        {
            if (smplLngthFix && candidate - oldLength < 0)
            {
                candidate = oldLength;
            }

            sound->setEnd(candidate);

            if (smplLngthFix)
            {
                sound->setStart(sound->getEnd() - oldLength);
            }

            displaySt();
            displayEnd();
            displayWave();
        }
    }
}

void TrimScreen::displayWave()
{
    const auto sound = sampler.lock()->getSound();

    if (!sound)
    {
        findWave()->setSampleData(nullptr, true, 0);
        findWave()->setSelection(0, 0);
        return;
    }

    const auto sampleData = sound->getSampleData();

    findWave()->setSampleData(sampleData, sound->isMono(), view);
    findWave()->setSelection(sound->getStart(), sound->getEnd());
}

void TrimScreen::displaySnd() const
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

void TrimScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void TrimScreen::displaySt() const
{
    if (sampler.lock()->getSoundCount() != 0)
    {
        const auto sound = sampler.lock()->getSound();
        findField("st")->setTextPadded(sound->getStart(), " ");
    }
    else
    {
        findField("st")->setTextPadded("0", " ");
    }
}

void TrimScreen::displayEnd() const
{
    if (sampler.lock()->getSoundCount() != 0)
    {
        const auto sound = sampler.lock()->getSound();
        findField("end")->setTextPadded(sound->getEnd(), " ");
    }
    else
    {
        findField("end")->setTextPadded("0", " ");
    }
}

void TrimScreen::displayView() const
{
    if (view == 0)
    {
        findField("view")->setText("LEFT");
    }
    else
    {
        findField("view")->setText("RIGHT");
    }
}

void TrimScreen::setView(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    view = i;
    displayView();
    displayWave();
}
