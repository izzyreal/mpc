#include "TrimScreen.hpp"

#include "hardware/Hardware.hpp"
#include "lcdgui/screens/LoopScreen.hpp"
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/Layer.hpp>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

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
	
    const auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }

    const auto focusedFieldName = focusedField->getName();

	if (focusedFieldName == "snd")
	{
		sampler->setPreviousScreenName("trim");
        mpc.getLayeredScreen()->openScreen<SoundScreen>();
	}
	else if (focusedFieldName == "st")
	{
        mpc.getLayeredScreen()->openScreen<StartFineScreen>();
	}
	else if (focusedFieldName == "end")
	{
        mpc.getLayeredScreen()->openScreen<EndFineScreen>();
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
		ls->showPopupForMs("Sorting by " + sampler->getSoundSortingTypeName(), 200);
		break;
	}
	case 1:
        mpc.getLayeredScreen()->openScreen<LoopScreen>();
		break;
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
		editSoundScreen->setReturnToScreenName("trim");
		
        mpc.getLayeredScreen()->openScreen<EditSoundScreen>();
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

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "st")
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
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
	else if (focusedFieldName == "snd" && i > 0)
	{
		sampler->selectNextSound();
		displaySnd();
		displayEnd();
		displayPlayX();
		displaySt();
		displayView();
		displayWave();
	}
	else if (focusedFieldName == "snd" && i < 0)
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
	if (!mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        return;
    }
    
	init();

    const auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }

    const auto focusedFieldName = focusedField->getName();

	if (focusedFieldName == "st")
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
    const auto loopLengthIsFixed = mpc.screens->get<LoopScreen>()->loopLngthFix;
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
	if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
	{
        mpc.getLayeredScreen()->openScreen<SaveScreen>();
		return;
	}

	init();

    auto focusedField = getFocusedField();

    if (!focusedField)
    {
        return;
    }
	
	if (!focusedField->isTypeModeEnabled())
    {
		return;
    }

	auto candidate = focusedField->enter();
	auto sound = sampler->getSound();
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	if (candidate != INT_MAX)
	{
        const auto focusedFieldName = focusedField->getName();

		if (focusedFieldName == "st" || focusedFieldName == "start")
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
		else if (focusedFieldName == "end")
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

	if (ls->getFocusedFieldName() == "dummy")
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
