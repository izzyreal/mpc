#include "LoopScreen.hpp"
#include "command/SplitLeftCommand.hpp"
#include "command/SplitRightCommand.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/Layer.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"

#include <climits>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

LoopScreen::LoopScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "loop", layerIndex)
{
    addChildT<Wave>()->setFine(false);
}

void LoopScreen::open()
{
    findField("loop")->setAlignment(Alignment::Centered);
    bool sound = sampler->getSound() ? true : false;

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

    ls->setFunctionKeysArrangement(sound ? 1 : 0);
}

void LoopScreen::openWindow()
{

    const auto focusedField = getFocusedField();
    auto sound = sampler->getSound();

    if (!focusedField || !sound)
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "snd")
    {
        sampler->setPreviousScreenName("loop");
        mpc.getLayeredScreen()->openScreen<SoundScreen>();
    }
    else if (focusedFieldName == "to")
    {
        mpc.getLayeredScreen()->openScreen<LoopToFineScreen>();
    }
    else if (focusedFieldName == "endlength" || focusedFieldName == "endlengthvalue")
    {
        mpc.getLayeredScreen()->openScreen<LoopEndFineScreen>();
    }
}

void LoopScreen::function(int f)
{

    switch (f)
    {
        case 0:
            mpc.getLayeredScreen()->openScreen<TrimScreen>();
            break;
        case 1:
        {
            sampler->switchToNextSoundSortType();
            ls->showPopupForMs("Sorting by " + sampler->getSoundSortingTypeName(), 200);
            break;
        }
        case 2:
            mpc.getLayeredScreen()->openScreen<ZoneScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<SndParamsScreen>();
            break;
        case 4:
        {
            if (sampler->getSoundCount() == 0)
            {
                return;
            }

            auto editSoundScreen = mpc.screens->get<EditSoundScreen>();
            editSoundScreen->setReturnToScreenName("loop");
            mpc.getLayeredScreen()->openScreen<EditSoundScreen>();
            break;
        }
        case 5:
            if (mpc.getHardware()->getButton(hardware::ComponentId::F6)->isPressed())
            {
                return;
            }

            sampler->playX();
            break;
    }
}

void LoopScreen::turnWheel(int i)
{

    auto soundInc = getSoundIncrement(i);
    auto sound = sampler->getSound();

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
        auto candidateLoopTo = sound->getLoopTo() + soundInc;
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
            auto newEndValue = sound->getEnd() + soundInc;

            auto trimScreen = mpc.screens->get<TrimScreen>();
            trimScreen->setEnd(newEndValue);
        }
        else
        {
            auto newLength = (sound->getEnd() - sound->getLoopTo()) + soundInc;
            setLength(newLength);
        }

        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
    else if (focusedFieldName == "playx")
    {
        sampler->setPlayX(sampler->getPlayX() + i);
        displayPlayX();
    }
    else if (focusedFieldName == "loop")
    {
        sampler->getSound()->setLoopEnabled(i > 0);
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
        sampler->selectNextSound();
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
        sampler->selectPreviousSound();
        displaySnd();
        displayPlayX();
        displayEndLength();
        displayEndLengthValue();
        displayLoop();
        displayTo();
        displayWave();
    }
}

void LoopScreen::setSlider(int i)
{
    if (!mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
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
            mpc.screens->get<TrimScreen>()->setSliderEnd(i);
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

void LoopScreen::setSliderLoopTo(int i)
{
    auto sound = sampler->getSound();
    auto const oldLength = sound->getEnd() - sound->getLoopTo();
    auto newLoopToValue = (int)((i / 124.0) * (loopLngthFix ? (sound->getFrameCount() - oldLength) : sound->getEnd()));
    setLoopTo(newLoopToValue);
}

void LoopScreen::setLoopTo(int newLoopToValue)
{
    const auto loopLengthIsFixed = loopLngthFix;
    const auto soundLengthIsFixed = mpc.screens->get<TrimScreen>()->smplLngthFix;
    auto sound = sampler->getSound();

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
void LoopScreen::setLength(int newLength)
{
    const auto loopLengthIsFixed = loopLngthFix;

    if (loopLengthIsFixed)
    {
        return;
    }

    const auto soundLengthIsFixed = mpc.screens->get<TrimScreen>()->smplLngthFix;

    if (newLength < 0)
    {
        newLength = 0;
    }

    auto sound = sampler->getSound();

    if (soundLengthIsFixed)
    {
        setLoopTo(sound->getEnd() - newLength);
    }
    else
    {
        auto trimScreen = mpc.screens->get<TrimScreen>();
        trimScreen->setEnd(sound->getLoopTo() + newLength);
    }
}

void LoopScreen::setSliderLength(int i)
{
    auto sound = sampler->getSound();
    auto newLength = (int)((i / 124.0) * sound->getFrameCount());
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
        mpc.getLayeredScreen()->openScreen<SaveScreen>();
        return;
    }

    const auto focusedField = getFocusedField();

    if (!focusedField || !focusedField->isTypeModeEnabled())
    {
        return;
    }

    auto candidate = focusedField->enter();
    auto sound = sampler->getSound();

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
        else if (focusedFieldName == "endlengthvalue" || focusedFieldName == "end")
        {
            if ((endSelected && focusedFieldName == "endlengthvalue") || focusedFieldName == "end")
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

void LoopScreen::displaySnd()
{
    auto sound = sampler->getSound();

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

void LoopScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void LoopScreen::displayTo()
{
    if (sampler->getSoundCount() != 0)
    {
        auto sound = sampler->getSound();
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

void LoopScreen::displayEndLength()
{
    findField("endlength")->setText(endSelected ? "  End" : "Lngth");
}

void LoopScreen::displayEndLengthValue()
{
    if (sampler->getSoundCount() == 0)
    {
        findField("endlengthvalue")->setTextPadded("0", " ");
        return;
    }

    auto sound = sampler->getSound();

    auto text = std::to_string(endSelected ? sound->getEnd() : sound->getEnd() - sound->getLoopTo());
    findField("endlengthvalue")->setTextPadded(text, " ");
}

void LoopScreen::displayLoop()
{
    if (sampler->getSoundCount() == 0)
    {
        findField("loop")->setText("OFF");
        return;
    }

    auto sound = sampler->getSound();
    findField("loop")->setText(sound->isLoopEnabled() ? "ON" : "OFF");
}

void LoopScreen::displayWave()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        findWave()->setSampleData(nullptr, true, 0);
        findWave()->setSelection(0, 0);
        return;
    }

    auto sampleData = sound->getSampleData();
    auto trimScreen = mpc.screens->get<TrimScreen>();
    findWave()->setSampleData(sampleData, sound->isMono(), trimScreen->view);
    findWave()->setSelection(sound->getLoopTo(), sound->getEnd());
}

void LoopScreen::setEndSelected(bool b)
{
    endSelected = b;
    displayEndLength();
    displayEndLengthValue();
}
