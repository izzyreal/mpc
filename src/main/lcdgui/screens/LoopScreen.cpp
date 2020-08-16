#include "LoopScreen.hpp"

#include <lcdgui/Layer.hpp>
#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

LoopScreen::LoopScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);

	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(false);

	baseControls->typableParams = { "to", "endlengthvalue" };
}

void LoopScreen::open()
{
	bool sound = sampler.lock()->getSound().lock() ? true : false;

	findField("snd").lock()->setFocusable(sound);
	findField("playx").lock()->setFocusable(sound);
	findField("to").lock()->setFocusable(sound);
	findField("to").lock()->enableTwoDots();
	findField("endlength").lock()->setFocusable(sound);
	findField("endlengthvalue").lock()->setFocusable(sound);
	findField("endlengthvalue").lock()->enableTwoDots();
	findField("loop").lock()->setFocusable(sound);
	findField("dummy").lock()->setFocusable(!sound);

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
	else if (param.compare("endlength") == 0 || param.compare("endlengthvalue") == 0)
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
			return;

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(mpc.screens->getScreenComponent("edit-sound"));
		editSoundScreen->setReturnToScreenName("loop");
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
		if (mpc.getControls().lock()->isF6Pressed())
			return;

		mpc.getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
}

void LoopScreen::turnWheel(int i)
{
    init();

	if (param == "")
		return;
	
    auto soundInc = getSoundIncrement(i);
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLoopLength = sound->getEnd() - sound->getLoopTo();
    
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));
	auto const loopFix = loopScreen->loopLngthFix;
	
	auto field = findField(param).lock();
	
	if (field->isSplit())
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("to") == 0)
	{
		auto candidateLoopTo = sound->getLoopTo() + soundInc;
		auto candidateEnd = candidateLoopTo + oldLoopLength;

		if (loopFix && candidateEnd > sound->getFrameCount())
		{
			candidateEnd = sound->getFrameCount();
			candidateLoopTo = sound->getFrameCount() - oldLoopLength;
		}

		if (!loopFix && candidateLoopTo > sound->getEnd())
		{
			sound->setLoopTo(sound->getEnd());
		}
		else
		{
			sound->setLoopTo(candidateLoopTo);
		}
		
		if (loopFix)
			sound->setEnd(candidateEnd);

		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
    }
    else if (param.compare("endlengthvalue") == 0)
	{
		if (endSelected)
		{
			auto endCandidate = sound->getEnd() + soundInc;

			if (endCandidate > sound->getFrameCount())
				endCandidate = sound->getFrameCount();

			sound->setEnd(endCandidate);

			if (loopFix)
			{
				auto loopToCandidate = sound->getEnd() - oldLoopLength;

				if (loopToCandidate < 0)
				{
					loopToCandidate = 0;
					sound->setEnd(oldLoopLength);
				}

				sound->setLoopTo(loopToCandidate);
			}
		}
		else {
			auto endCandidate = sound->getEnd() + soundInc;
			
			if (endCandidate < sound->getLoopTo())
				endCandidate = sound->getLoopTo();

			if (endCandidate > sound->getFrameCount())
				endCandidate = sound->getFrameCount();

			sound->setEnd(endCandidate);
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
		return;

	init();

	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();
	auto const lengthFix = trimScreen->smplLngthFix;

	auto candidatePos = (int)((i / 124.0) * sound->getFrameCount());
	
	if (param.compare("to") == 0)
	{
		if (candidatePos < 0)
			candidatePos = 0;
		
		if (loopLngthFix && candidatePos + oldLength > sound->getFrameCount())
				candidatePos = sound->getFrameCount() - oldLength;
		
		sound->setLoopTo(candidatePos);

		if (lengthFix)
			sound->setEnd(sound->getLoopTo() + oldLength);
		
		displayEndLength();
		displayEndLengthValue();
		displayTo();
		displayWave();
	}
	else if (param.compare("endlengthvalue") == 0)
	{
		auto maxEndPos = lengthFix ? oldLength : 0;

		if (candidatePos < maxEndPos)
			candidatePos = maxEndPos;

		sound->setEnd(candidatePos);

		if (lengthFix)
			sound->setLoopTo(sound->getEnd() - oldLength);

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
		
	auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();

	if (!field->isTypeModeEnabled())
		return;

	auto candidate = field->enter();
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();

	if (candidate != INT_MAX)
	{
		if (param.compare("to") == 0)
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
		else if (param.compare("endlengthvalue") == 0 || param.compare("end") == 0)
		{
			if ((endSelected && param.compare("endlengthvalue") == 0) || param.compare("end") == 0)
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
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
	findWave().lock()->setSampleData(sampleData, sound->isMono(), trimScreen->view);
	findWave().lock()->setSelection(sound->getLoopTo(), sound->getEnd());
}

void LoopScreen::setEndSelected(bool b)
{
	endSelected = b;
	displayEndLength();
	displayEndLengthValue();
}
