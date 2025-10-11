#include "LoopScreen.hpp"

#include <lcdgui/Layer.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::controls;

LoopScreen::LoopScreen(mpc::Mpc& mpc, const int layerIndex)
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
	init();
	
	if (param == "snd")
	{
		sampler->setPreviousScreenName("loop");
		openScreen("sound");
	}
	else if (param == "to")
	{
		openScreen("loop-to-fine");
	}
	else if (param == "endlength" || param == "endlengthvalue")
	{
		openScreen("loop-end-fine");
	}
}

void LoopScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		openScreen("trim");
		break;
	case 1:
	{
        sampler->switchToNextSoundSortType();
		openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Sorting by " + sampler->getSoundSortingTypeName());
		popupScreen->returnToScreenAfterMilliSeconds("loop", 200);
		break;
	}
	case 2:
		openScreen("zone");
		break;
	case 3:
		openScreen("params");
		break;
	case 4:
	{
		if (sampler->getSoundCount() == 0)
			return;

		auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
		editSoundScreen->setReturnToScreenName("loop");
		openScreen("edit-sound");
		break;
	}
	case 5:
		if (mpc.getControls()->isF6Pressed())
			return;

		mpc.getControls()->setF6Pressed(true);
		sampler->playX();
		break;
	}
}

void LoopScreen::turnWheel(int i)
{
    init();

    auto soundInc = getSoundIncrement(i);
	auto sound = sampler->getSound();

	if (param.empty() || !sound)
    {
        return;
    }

	auto field = findField(param);
	
	if (field->isSplit())
    {
        soundInc = field->getSplitIncrement(i >= 0);
    }

	if (field->isTypeModeEnabled())
    {
        field->disableTypeMode();
    }

	if (param == "to")
	{
		auto candidateLoopTo = sound->getLoopTo() + soundInc;
        setLoopTo(candidateLoopTo);
        displayEndLength();
        displayEndLengthValue();
        displayTo();
        displayWave();
    }
    else if (param == "endlengthvalue")
	{
		if (endSelected)
		{
            auto newEndValue = sound->getEnd() + soundInc;

            auto trimScreen = mpc.screens->get<TrimScreen>("trim");
            trimScreen->setEnd(newEndValue);
		}
		else {
			auto newLength = (sound->getEnd() - sound->getLoopTo()) + soundInc;
            setLength(newLength);
		}

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
    }
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
	else if (param == "loop")
	{
        sampler->getSound()->setLoopEnabled(i > 0);
        displayLoop();
	}
	else if (param == "endlength")
	{
		setEndSelected(i > 0);
		displayEndLength();
		displayEndLengthValue();
	}
	else if (param == "snd" && i > 0)
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
	else if (param == "snd" && i < 0)
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
	if (!mpc.getControls()->isShiftPressed())
    {
        return;
    }

	init();

    auto trimScreen = mpc.screens->get<TrimScreen>("trim");

    if (param == "to")
	{
        setSliderLoopTo(i);
		
		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
	}
    else if (param == "endlengthvalue")
    {
        if (endSelected)
        {
            trimScreen->setSliderEnd(i);
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
    const auto soundLengthIsFixed = mpc.screens->get<TrimScreen>("trim")->smplLngthFix;
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

    if (newLoopToValue < lowerBound) newLoopToValue = lowerBound;
    if (newLoopToValue > upperBound) newLoopToValue = upperBound;

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

    const auto soundLengthIsFixed = mpc.screens->get<TrimScreen>("trim")->smplLngthFix;

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
        auto trimScreen = mpc.screens->get<TrimScreen>("trim");
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
    splitLeft();
}

void LoopScreen::right()
{
	splitRight();
}

void LoopScreen::pressEnter()
{
	if (mpc.getControls()->isShiftPressed())
	{
		openScreen("save");
		return;
	}
	
	init();

	auto field = ls->getFocusedLayer()->findField(param);

	if (!field->isTypeModeEnabled())
		return;

	auto candidate = field->enter();
	auto sound = sampler->getSound();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();

	if (candidate != INT_MAX)
	{
		if (param == "to")
		{
			if (loopLngthFix && candidate + oldLength > sound->getFrameCount())
				candidate = sound->getFrameCount() - oldLength;

			if (candidate > sound->getEnd() && !loopLngthFix)
				candidate = sound->getEnd();

			sound->setLoopTo(candidate);
			displayTo();

			if (loopLngthFix)
				sound->setEnd(sound->getLoopTo() + oldLength);

			displayEndLengthValue();
			displayEndLength();
			displayWave();
		}
		else if (param == "endlengthvalue" || param == "end")
		{
			if ((endSelected && param == "endlengthvalue") || param == "end")
			{
				if (loopLngthFix && candidate - oldLength < 0)
					candidate = oldLength;

				if (candidate > sound->getFrameCount())
					candidate = sound->getFrameCount();

				sound->setEnd(candidate);

				if (loopLngthFix)
					sound->setLoopTo(sound->getEnd() - oldLength);
			}
			else {
				auto endCandidate = sound->getLoopTo() + candidate;

				// No need to check the < case
				if (endCandidate > sound->getFrameCount())
					endCandidate = sound->getFrameCount();
				
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

	if (ls->getFocus() == "dummy")
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
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	findWave()->setSampleData(sampleData, sound->isMono(), trimScreen->view);
	findWave()->setSelection(sound->getLoopTo(), sound->getEnd());
}

void LoopScreen::setEndSelected(bool b)
{
	endSelected = b;
	displayEndLength();
	displayEndLengthValue();
}
