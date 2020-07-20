#include "LoopScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

LoopScreen::LoopScreen(const int layerIndex)
	: ScreenComponent("loop", layerIndex)
{
	addChild(move(make_shared<TwoDots>()));
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(false);

	auto twoDots = findTwoDots().lock();
	twoDots->setVisible(0, true);
	twoDots->setVisible(1, true);
	twoDots->setVisible(2, false);
	twoDots->setVisible(3, false);

	typableParams = vector<string>{ "to", "endlengthvalue" };
}

void LoopScreen::open()
{
	bool sound = sampler.lock()->getSound().lock() ? true : false;

	findField("snd").lock()->setFocusable(sound);
	findField("playx").lock()->setFocusable(sound);
	findField("to").lock()->setFocusable(sound);
	findField("endlength").lock()->setFocusable(sound);
	findField("endlengthvalue").lock()->setFocusable(sound);
	findField("loop").lock()->setFocusable(sound);
	findField("dummy").lock()->setFocusable(!sound);

	if (sound)
	{
		init();
		if (param.compare("dummy") == 0)
		{
			ls.lock()->setFocus("snd");
		}
	}
	else
	{
		ls.lock()->setFocus("dummy");
	}

	displaySnd();
	displayPlayX();
	displayEndLength();
	displayEndLengthValue();
	displayLoop();
	displayTo();
	displayWave();
}

void LoopScreen::openWindow()
{
	init();
	
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("loop");
		ls.lock()->openScreen("sound");
	}
	else if (param.compare("to") == 0)
	{
		ls.lock()->openScreen("loop-to-fine");
	}
	else if (param.compare("endlengthvalue") == 0)
	{
		ls.lock()->openScreen("loop-end-fine");
	}
}

void LoopScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		ls.lock()->openScreen("trim");
		break;
	case 1:
		sampler.lock()->sort();
		break;
	case 2:
		ls.lock()->openScreen("zone");
		break;
	case 3:
		ls.lock()->openScreen("params");
		break;
	case 4:
	{
		if (sampler.lock()->getSoundCount() == 0)
		{
			return;
		}

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(Screens::getScreenComponent("edit-sound"));
		editSoundScreen->setPreviousScreenName("loop");
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
}

void LoopScreen::turnWheel(int i)
{
    init();

	if (param == "")
	{
		return;
	}
	
    auto soundInc = getSoundIncrement(i);
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLoopLength = sound->getEnd() - sound->getLoopTo();
    
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(Screens::getScreenComponent("loop"));
	auto const loopFix = loopScreen->loopLngthFix;
	
	auto mtf = findField(param).lock();
	
	if (mtf->isSplit())
	{
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}

	if (param.compare("to") == 0)
	{
		if (loopFix && sound->getLoopTo() + soundInc + oldLoopLength > sound->getLastFrameIndex())
		{
			return;
		}

        sound->setLoopTo(sound->getLoopTo() + soundInc);
		
		if (loopFix)
		{
			sound->setEnd(sound->getLoopTo() + oldLoopLength);
		}

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
    }
    else if (param.compare("endlengthvalue") == 0)
	{
		if (loopFix && sound->getEnd() + soundInc - oldLoopLength < 0)
		{
			return;
		}
        
		sound->setEnd(sound->getEnd() + soundInc);
		
		if (loopFix)
		{
			sound->setLoopTo(sound->getEnd() - oldLoopLength);
		}

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
    }
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("loop") == 0)
	{
		sound->setLoopEnabled(i > 0);
		displayLoop();
	}
	else if (param.compare("endlength") == 0)
	{
		setEndSelected(i > 0);
		displayEndLength();
		displayEndLengthValue();
	}
	else if (param.compare("snd") == 0 && i > 0)
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
	else if (param.compare("snd") == 0 && i < 0)
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

void LoopScreen::setSlider(int i)
{
	if (!mpc.getControls().lock()->isShiftPressed())
	{
		return;
	}

	init();

	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();
	auto const lengthFix = trimScreen->smplLngthFix;

	auto candidatePos = (int)((i / 124.0) * sound->getLastFrameIndex());
	auto maxPos = 0;

	if (param.compare("to") == 0)
	{
		maxPos = lengthFix ? sound->getLastFrameIndex() - oldLength : sound->getLastFrameIndex();
		if (candidatePos > maxPos)
		{
			candidatePos = maxPos;
		}
		
		sound->setLoopTo(candidatePos);

		if (lengthFix)
		{
			sound->setEnd(sound->getLoopTo() + oldLength);
		}

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
	}
	else if (param.compare("endlengthvalue") == 0)
	{
		maxPos = lengthFix ? oldLength : 0;
		
		if (candidatePos < maxPos)
		{
			candidatePos = maxPos;
		}

		sound->setEnd(candidatePos);

		if (lengthFix)
		{
			sound->setLoopTo(sound->getEnd() - oldLength);
		}

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
	}
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
	init();
	
	if (!isTypable())
	{
		return;
	}
	
	auto lLs = ls.lock();
	auto mtf = findField(param).lock();

	if (!mtf->isTypeModeEnabled())
	{
		return;
	}

	auto candidate = mtf->enter();
	auto sound = sampler.lock()->getSound().lock();
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(Screens::getScreenComponent("loop"));

	auto const oldLength = sound->getEnd() - sound->getLoopTo();
	auto const lengthFix = loopScreen->loopLngthFix;

	if (candidate != INT_MAX)
	{
		if (param.compare("to") == 0)
		{
			if (lengthFix && candidate + oldLength > sound->getLastFrameIndex())
			{
				return;
			}

			sound->setLoopTo(candidate);

			if (lengthFix)
			{
				sound->setEnd(sound->getLoopTo() + oldLength);
			}
			displayEndLengthValue();
			displayEndLength();
			displayWave();
		}
		else if (param.compare("endlengthvalue") == 0)
		{
			if (lengthFix && candidate - oldLength < 0)
			{
				return;
			}

			sound->setEnd(candidate);
			if (lengthFix)
			{
				sound->setLoopTo(sound->getEnd() - oldLength);
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
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findField("snd").lock()->setText("(no sound)");
		ls.lock()->setFocus("dummy");
		return;
	}

	if (ls.lock()->getFocus().compare("dummy") == 0)
	{
		ls.lock()->setFocus(findField("snd").lock()->getName());
	}

	auto sampleName = sound->getName();

	if (!sound->isMono())
	{
		sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";
	}

	findField("snd").lock()->setText(sampleName);
}

void LoopScreen::displayPlayX()
{
	findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void LoopScreen::displayTo()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		auto sound = sampler.lock()->getSound().lock();
		findField("to").lock()->setTextPadded(sound->getLoopTo(), " ");
	}
	else
	{
		findField("to").lock()->setTextPadded("0", " ");
	}

	if (!endSelected)
	{
		displayEndLengthValue();
	}
}

void LoopScreen::displayEndLength()
{
	findField("endlength").lock()->setText(endSelected ? "  End" : "Lngth");
}

void LoopScreen::displayEndLengthValue()
{
	if (sampler.lock()->getSoundCount() == 0)
	{
		findField("endlengthvalue").lock()->setTextPadded("0", " ");
		return;
	}

	auto sound = sampler.lock()->getSound().lock();

	auto text = to_string(endSelected ? sound->getEnd() : sound->getEnd() - sound->getLoopTo());
	findField("endlengthvalue").lock()->setTextPadded(text, " ");
}

void LoopScreen::displayLoop()
{
	if (sampler.lock()->getSoundCount() == 0)
	{
		findField("loop").lock()->setText("OFF");
		return;
	}

	auto sound = sampler.lock()->getSound().lock();
	findField("loop").lock()->setText(sound->isLoopEnabled() ? "ON" : "OFF");
}

void LoopScreen::displayWave()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findWave().lock()->setSampleData(nullptr, true, 0);
		findWave().lock()->setSelection(0, 0);
		return;
	}

	auto sampleData = sound->getSampleData();
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));
	findWave().lock()->setSampleData(sampleData, sound->isMono(), trimScreen->view);
	findWave().lock()->setSelection(sound->getLoopTo(), sound->getEnd());
}

void LoopScreen::setEndSelected(bool b)
{
	endSelected = b;
	displayEndLength();
	displayEndLengthValue();
}
