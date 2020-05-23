#include "LoopScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::lcdgui::screens;
using namespace std;

LoopScreen::LoopScreen(const int layerIndex)
	: ScreenComponent("loop", layerIndex)
{
}

void LoopScreen::open()
{
	typableParams = vector<string>{ "to", "endlengthvalue" };
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
		ls.lock()->openScreen("looptofine");
	}
	else if (param.compare("endlengthvalue") == 0)
	{
		ls.lock()->openScreen("loopendfine");
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
		
		auto newSoundName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSoundName = sampler.lock()->addOrIncreaseNumber(newSoundName);
		auto editSoundGui = mpc.getUis().lock()->getEditSoundGui();
		editSoundGui->setNewName(newSoundName);
		editSoundGui->setPreviousScreenName("loop");
		ls.lock()->openScreen("editsound");
		break;
	}
	case 5:
	{
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		vector<int> zone{ soundGui->getZoneStart(soundGui->getZoneNumber()), soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		sampler.lock()->playX(soundGui->getPlayX(), &zone);
		break;
	}
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
    
	auto zoomGui = mpc.getUis().lock()->getZoomGui();
	auto const loopFix = zoomGui->isLoopLngthFix();
	
	auto mtf = ls.lock()->lookupField(param).lock();
	
	if (mtf->isSplit())
	{
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}

	auto soundGui = mpc.getUis().lock()->getSoundGui();
	
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
    }
	else if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("loop") == 0) {
		sampler.lock()->setLoopEnabled(sampler.lock()->getSoundIndex(), i > 0);
	}
	else if (param.compare("endlength") == 0) {
		soundGui->setEndSelected(i > 0);
	}
	else if (param.compare("snd") == 0 && i > 0) {
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		sampler.lock()->setSoundGuiPrevSound();
	}
}

void LoopScreen::setSlider(int i)
{
	if (!mpc.getControls().lock()->isShiftPressed())
	{
		return;
	}

	init();

	auto zoomGui = mpc.getUis().lock()->getZoomGui();
	auto sound = sampler.lock()->getSound().lock();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();
	auto const lengthFix = zoomGui->isSmplLngthFix();

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
	auto mtf = ls.lock()->lookupField(param).lock();

	if (!mtf->isTypeModeEnabled())
	{
		return;
	}

	auto candidate = mtf->enter();
	auto sound = sampler.lock()->getSound().lock();
	auto zoomGui = mpc.getUis().lock()->getZoomGui();

	auto const oldLength = sound->getEnd() - sound->getLoopTo();
	auto const lengthFix = zoomGui->isLoopLngthFix();

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
		}
	}
}
