#include "LoopScreen.hpp"
#include "Mpc.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/TrimScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"

#include <climits>

#include "StrUtil.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

LoopScreen::LoopScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop", layerIndex)
{
    addChildT<Wave>()->setFine(false);
}

void LoopScreen::open()
{
    findField("loop")->setAlignment(Alignment::Centered);
    const bool sound = sampler.lock()->getSound() ? true : false;

    findField("snd")->setFocusable(sound);
    findField("playx")->setFocusable(sound);
    findField("to")->setFocusable(sound);
    findField("to")->enableTwoDots();
    findField("endlength")->setFocusable(sound);
    findField("endlengthvalue")->setFocusable(sound);
    findField("endlengthvalue")->enableTwoDots();
    findField("loop")->setFocusable(sound);
    findField("dummy")->setFocusable(!sound);

    displaySnd();
    displayPlayX();
    displayEndLength();
    displayEndLengthValue();
    displayLoop();
    displayTo();
    displayWave();

    ls.lock()->setFunctionKeysArrangement(sound ? 1 : 0);
}

void LoopScreen::openWindow()
{

    const auto focusedField = getFocusedField();
    const auto sound = sampler.lock()->getSound();

    if (!focusedField || !sound)
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "snd")
    {
        sampler.lock()->setPreviousScreenName("loop");
        openScreenById(ScreenId::SoundScreen);
    }
    else if (focusedFieldName == "to")
    {
        openScreenById(ScreenId::LoopToFineScreen);
    }
    else if (focusedFieldName == "endlength" ||
             focusedFieldName == "endlengthvalue")
    {
        openScreenById(ScreenId::LoopEndFineScreen);
    }
}

void LoopScreen::function(const int f)
{
    switch (f)
    {
        case 0:
            openScreenById(ScreenId::TrimScreen);
            break;
        case 1:
        {
            sampler.lock()->switchToNextSoundSortType();
            ls.lock()->showPopupForMs(
                "Sorting by " + sampler.lock()->getSoundSortingTypeName(), 200);
            break;
        }
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
            editSoundScreen->setReturnToScreenName("loop");
            openScreenById(ScreenId::EditSoundScreen);
            break;
        }
        case 5:
            if (mpc.getHardware()
                    ->getButton(hardware::ComponentId::F6)
                    ->isPressed())
            {
                return;
            }

            sampler.lock()->playX();
            break;
        default:;
    }
}

void LoopScreen::turnWheel(const int i)
{
    auto soundInc = getSoundIncrement(i);
    const auto sound = sampler.lock()->getSound();

    const auto focusedField = getFocusedField();

    if (!focusedField || !sound)
    {
        return;
    }

    if (focusedField->isSplit())
    {
        soundInc = focusedField->getSplitIncrement(i >= 0);
    }

    if (focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "to")
    {
        const auto candidateLoopTo = sound->getLoopTo() + soundInc;
        setLoopTo(candidateLoopTo);
        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
    else if (focusedFieldName == "endlengthvalue")
    {
        if (endSelected)
        {
            const auto newEndValue = sound->getEnd() + soundInc;

            const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
            trimScreen->setEnd(newEndValue);
        }
        else
        {
            const auto newLength =
                sound->getEnd() - sound->getLoopTo() + soundInc;
            setLength(newLength);
        }

        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "loop")
    {
        sampler.lock()->getSound()->setLoopEnabled(i > 0);
        displayLoop();
    }
    else if (focusedFieldName == "endlength")
    {
        setEndSelected(i > 0);
        displayEndLength();
        displayEndLengthValue();
    }
    else if (focusedFieldName == "snd" && i > 0)
    {
        sampler.lock()->selectNextSound();
        displaySnd();
        displayPlayX();
        displayEndLength();
        displayEndLengthValue();
        displayLoop();
        displayTo();
        displayWave();
    }
    else if (focusedFieldName == "snd" && i < 0)
    {
        sampler.lock()->selectPreviousSound();
        displaySnd();
        displayPlayX();
        displayEndLength();
        displayEndLengthValue();
        displayLoop();
        displayTo();
        displayWave();
    }
}

void LoopScreen::setSlider(const int i)
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

    if (focusedFieldName == "to")
    {
        setSliderLoopTo(i);

        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
    else if (focusedFieldName == "endlengthvalue")
    {
        if (endSelected)
        {
            mpc.screens->get<ScreenId::TrimScreen>()->setSliderEnd(i);
        }
        else
        {
            setSliderLength(i);
        }

        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
}

void LoopScreen::setSliderLoopTo(const int i) const
{
    const auto sound = sampler.lock()->getSound();
    auto const oldLength = sound->getEnd() - sound->getLoopTo();
    const auto newLoopToValue = static_cast<int>(
        i / 124.0 *
        (loopLngthFix ? sound->getFrameCount() - oldLength : sound->getEnd()));
    setLoopTo(newLoopToValue);
}

void LoopScreen::setLoopTo(int newLoopToValue) const
{
    const auto loopLengthIsFixed = loopLngthFix;
    const auto soundLengthIsFixed =
        mpc.screens->get<ScreenId::TrimScreen>()->isSampleLengthFixed();
    const auto sound = sampler.lock()->getSound();

    const auto oldSoundLength = sound->getEnd() - sound->getStart();
    const auto oldLoopLength = sound->getEnd() - sound->getLoopTo();

    auto lowerBound = 0;
    auto upperBound = sound->getEnd();

    if (soundLengthIsFixed && loopLengthIsFixed)
    {
        lowerBound = oldSoundLength - oldLoopLength;
    }

    if (loopLengthIsFixed)
    {
        upperBound = sound->getFrameCount() - oldLoopLength;
    }

    if (newLoopToValue < lowerBound)
    {
        newLoopToValue = lowerBound;
    }
    if (newLoopToValue > upperBound)
    {
        newLoopToValue = upperBound;
    }

    sound->setLoopTo(newLoopToValue);

    if (loopLengthIsFixed)
    {
        sound->setEnd(sound->getLoopTo() + oldLoopLength);
    }

    if (soundLengthIsFixed)
    {
        sound->setStart(sound->getEnd() - oldSoundLength);
    }
}

// Adjusts the Loop To value if soundLengthIsFixed, else adjusts the End value
void LoopScreen::setLength(int newLength) const
{
    if (loopLngthFix)
    {
        return;
    }

    const auto soundLengthIsFixed =
        mpc.screens->get<ScreenId::TrimScreen>()->isSampleLengthFixed();

    if (newLength < 0)
    {
        newLength = 0;
    }

    const auto sound = sampler.lock()->getSound();

    if (soundLengthIsFixed)
    {
        setLoopTo(sound->getEnd() - newLength);
    }
    else
    {
        const auto trimScreen = mpc.screens->get<ScreenId::TrimScreen>();
        trimScreen->setEnd(sound->getLoopTo() + newLength);
    }
}

bool LoopScreen::isLoopLengthFixed() const
{
    return loopLngthFix;
}

void LoopScreen::setLoopLengthFixed(const bool b)
{
    loopLngthFix = b;
}

void LoopScreen::setSliderLength(const int i) const
{
    const auto sound = sampler.lock()->getSound();
    const auto newLength = static_cast<int>(i / 124.0 * sound->getFrameCount());
    setLength(newLength);
}

void LoopScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void LoopScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void LoopScreen::pressEnter()
{
    if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        openScreenById(ScreenId::SaveScreen);
        return;
    }

    const auto focusedField = getFocusedField();

    if (!focusedField || !focusedField->isTypeModeEnabled())
    {
        return;
    }

    auto candidate = focusedField->enter();
    const auto sound = sampler.lock()->getSound();

    auto const oldLength = sound->getEnd() - sound->getLoopTo();

    const auto focusedFieldName = focusedField->getName();

    if (candidate != INT_MAX)
    {
        if (focusedFieldName == "to")
        {
            if (loopLngthFix && candidate + oldLength > sound->getFrameCount())
            {
                candidate = sound->getFrameCount() - oldLength;
            }

            if (candidate > sound->getEnd() && !loopLngthFix)
            {
                candidate = sound->getEnd();
            }

            sound->setLoopTo(candidate);
            displayTo();

            if (loopLngthFix)
            {
                sound->setEnd(sound->getLoopTo() + oldLength);
            }

            displayEndLengthValue();
            displayEndLength();
            displayWave();
        }
        else if (focusedFieldName == "endlengthvalue" ||
                 focusedFieldName == "end")
        {
            if ((endSelected && focusedFieldName == "endlengthvalue") ||
                focusedFieldName == "end")
            {
                if (loopLngthFix && candidate - oldLength < 0)
                {
                    candidate = oldLength;
                }

                if (candidate > sound->getFrameCount())
                {
                    candidate = sound->getFrameCount();
                }

                sound->setEnd(candidate);

                if (loopLngthFix)
                {
                    sound->setLoopTo(sound->getEnd() - oldLength);
                }
            }
            else
            {
                auto endCandidate = sound->getLoopTo() + candidate;

                // No need to check the < case
                if (endCandidate > sound->getFrameCount())
                {
                    endCandidate = sound->getFrameCount();
                }

                sound->setEnd(endCandidate);
            }

            displayEndLength();
            displayEndLengthValue();
            displayTo();
            displayWave();
        }
    }
}

void LoopScreen::displaySnd() const
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

void LoopScreen::displayPlayX() const
{
    findField("playx")->setText(playXNames[sampler.lock()->getPlayX()]);
}

void LoopScreen::displayTo() const
{
    if (sampler.lock()->getSoundCount() != 0)
    {
        const auto sound = sampler.lock()->getSound();
        findField("to")->setTextPadded(sound->getLoopTo(), " ");
    }
    else
    {
        findField("to")->setTextPadded("0", " ");
    }

    if (!endSelected)
    {
        displayEndLengthValue();
    }
}

void LoopScreen::displayEndLength() const
{
    findField("endlength")->setText(endSelected ? "  End" : "Lngth");
}

void LoopScreen::displayEndLengthValue() const
{
    if (sampler.lock()->getSoundCount() == 0)
    {
        findField("endlengthvalue")->setTextPadded("0", " ");
        return;
    }

    const auto sound = sampler.lock()->getSound();

    const auto text = std::to_string(
        endSelected ? sound->getEnd() : sound->getEnd() - sound->getLoopTo());
    findField("endlengthvalue")->setTextPadded(text, " ");
}

void LoopScreen::displayLoop() const
{
    if (sampler.lock()->getSoundCount() == 0)
    {
        findField("loop")->setText("OFF");
        return;
    }

    const auto sound = sampler.lock()->getSound();
    findField("loop")->setText(sound->isLoopEnabled() ? "ON" : "OFF");
}

void LoopScreen::displayWave()
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
    findWave()->setSampleData(sampleData, sound->isMono(),
                              trimScreen->getView());
    findWave()->setSelection(sound->getLoopTo(), sound->getEnd());
}

void LoopScreen::setEndSelected(const bool b)
{
    endSelected = b;
    displayEndLength();
    displayEndLengthValue();
}
