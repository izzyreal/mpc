#include "TrimScreen.hpp"

#include "hardware/Hardware.h"
#include "lcdgui/screens/LoopScreen.hpp"
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/Layer.hpp>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::controls;

TrimScreen::TrimScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "trim", layerIndex)
{
	addChildT<Wave>()->setFine(false);
}

void TrimScreen::open()
{
    findField("view")->setAlignment(Alignment::Centered);
	bool sound = sampler->getSound() ? true : false;

	findField("snd")->setFocusable(sound);
	findField("playx")->setFocusable(sound);
	findField("st")->setFocusable(sound);
	findField("st")->enableTwoDots();
	findField("end")->setFocusable(sound);
	findField("end")->enableTwoDots();
	findField("view")->setFocusable(sound);
	findField("dummy")->setFocusable(!sound);

	displaySnd();
	displayPlayX();
	displaySt();
	displayEnd();
	displayView();
	displayWave();

	ls->setFunctionKeysArrangement(sound ? 1 : 0);
}

void TrimScreen::openWindow()
{
	init();
	
	if (param == "snd")
	{
		sampler->setPreviousScreenName("trim");
		openScreen("sound");
	}
	else if (param == "st")
	{
		openScreen("start-fine");
	}
	else if (param == "end")
	{
		openScreen("end-fine");
	}
}

void TrimScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
	{
        sampler->switchToNextSoundSortType();
		openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Sorting by " + sampler->getSoundSortingTypeName());
		popupScreen->returnToScreenAfterMilliSeconds("trim", 200);
		break;
	}
	case 1:
		openScreen("loop");
		break;
	case 2:
		openScreen("zone");
		break;
	case 3:
		openScreen("params");
		break;
	case 4:
	{
		if (sampler->getSoundCount() == 0)
		{
			return;
		}

		auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
		editSoundScreen->setReturnToScreenName("trim");
		
		openScreen("edit-sound");
		break;
	}
	case 5:
	{
		sampler->playX();
		break;
	}
	}
}

void TrimScreen::turnWheel(int i)
{
	init();

	auto sound = sampler->getSound();

	if (param.empty() || !sound)
    {
        return;
    }

	auto const oldLength = sound->getEnd() - sound->getStart();
	
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param);
	
	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param == "st")
	{
		if (smplLngthFix && sound->getStart() + soundInc + oldLength > sound->getFrameCount())
			return;
		
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
	else if (param == "end")
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
	else if (param == "view")
	{
		setView(view + i);
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
	else if (param == "snd" && i > 0)
	{
		sampler->selectNextSound();
		displaySnd();
		displayEnd();
		displayPlayX();
		displaySt();
		displayView();
		displayWave();
	}
	else if (param == "snd" && i < 0)
	{
		sampler->selectPreviousSound();
		displaySnd();
		displayEnd();
		displayPlayX();
		displaySt();
		displayView();
		displayWave();
	}
}

void TrimScreen::setSlider(int i)
{
	if (!mpc.getHardware()->getButton("shift")->isPressed())
    {
        return;
    }
    
	init();

	if (param == "st")
	{
        setSliderStart(i);
        displayWave();
	}
	else if (param == "end")
	{
        setSliderEnd(i);
		displayWave();
	}
}

void TrimScreen::setSliderStart(int i)
{
    auto sound = sampler->getSound();
    auto const oldLength = sound->getEnd() - sound->getStart();
    auto candidatePos = (int) ((i / 124.0) * sound->getFrameCount());

    auto maxPos = smplLngthFix ? sound->getFrameCount() - oldLength : sound->getFrameCount();

    if (candidatePos > maxPos)
        candidatePos = maxPos;

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

void TrimScreen::setSliderEnd(int i)
{
    auto sound = sampler->getSound();
    auto newValue = (int)((i / 124.0) * sound->getFrameCount());
    setEnd(newValue);
    displayEnd();
}

void TrimScreen::setEnd(int newValue)
{
    const auto loopLengthIsFixed = mpc.screens->get<LoopScreen>("loop")->loopLngthFix;
    auto sound = sampler->getSound();

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

    if (newValue < lowerBound) newValue = lowerBound;
    if (newValue > upperBound) newValue = upperBound;

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
	if (mpc.getHardware()->getButton("shift")->isPressed())
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
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	if (candidate != INT_MAX)
	{
		if (param == "st" || param == "start")
		{
			if (smplLngthFix && candidate + oldLength > sound->getFrameCount())
				candidate = sound->getFrameCount() - oldLength;

			sound->setStart(candidate);

			if (smplLngthFix)
				sound->setEnd(sound->getStart() + oldLength);

			displaySt();
			displayEnd();
			displayWave();
		}
		else if (param == "end")
		{
			if (smplLngthFix && candidate - oldLength < 0)
				candidate = oldLength;

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
	auto sound = sampler->getSound();

	if (!sound)
	{
		findWave()->setSampleData(nullptr, true, 0);
		findWave()->setSelection(0, 0);
		return;
	}

	auto sampleData = sound->getSampleData();
	
	findWave()->setSampleData(sampleData, sound->isMono(), view);
	findWave()->setSelection(sound->getStart(), sound->getEnd());
}

void TrimScreen::displaySnd()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		findField("snd")->setText("(no sound)");
		ls->setFocus("dummy");
		return;
	}

	if (ls->getFocus() == "dummy")
		ls->setFocus("snd");

	auto sampleName = sound->getName();

	if (!sound->isMono())
	{
		sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";
	}
	findField("snd")->setText(sampleName);
}


void TrimScreen::displayPlayX()
{
	findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void TrimScreen::displaySt()
{
	if (sampler->getSoundCount() != 0)
	{
		auto sound = sampler->getSound();
		findField("st")->setTextPadded(sound->getStart(), " ");
	}
	else {
		findField("st")->setTextPadded("0", " ");
	}
}

void TrimScreen::displayEnd()
{
	if (sampler->getSoundCount() != 0)
	{
		auto sound = sampler->getSound();
		findField("end")->setTextPadded(sound->getEnd(), " ");
	}
	else {
		findField("end")->setTextPadded("0", " ");
	}
}

void TrimScreen::displayView()
{
	if (view == 0)
	{
		findField("view")->setText("LEFT");
	}
	else {
		findField("view")->setText("RIGHT");
	}
}

void TrimScreen::setView(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	
	view = i;
	displayView();
	displayWave();
}
