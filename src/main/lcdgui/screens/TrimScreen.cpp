#include "TrimScreen.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

TrimScreen::TrimScreen(const int layerIndex)
	: ScreenComponent("trim", layerIndex)
{
}

void TrimScreen::open()
{
	typableParams = vector<string>{ "st", "end" };
}

void TrimScreen::openWindow()
{
	init();
	
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("trim");
		ls.lock()->openScreen("sound");
	}
	else if (param.compare("st") == 0) {
		ls.lock()->openScreen("startfine");
	}
	else if (param.compare("end") == 0) {
		ls.lock()->openScreen("endfine");
	}
}

void TrimScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		sampler.lock()->sort();
		break;
	case 1:
		ls.lock()->openScreen("loop");
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
		auto editSoundGui = mpc.getUis().lock()->getEditSoundGui();
		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		newSampleName = sampler.lock()->addOrIncreaseNumber(StrUtil::trim(newSampleName));

		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("trim");
		
		ls.lock()->openScreen("editsound");
		break;
	}
	case 5:
	{
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		auto zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()), soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		sampler.lock()->playX(soundGui->getPlayX(), &zone);
		break;
	}
	}
}

void TrimScreen::turnWheel(int i)
{
	init();

	if (param == "")
	{
		return;
	}
	
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto zoomGui = mpc.getUis().lock()->getZoomGui();
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
	auto const lengthFix = zoomGui->isSmplLngthFix();
	
	//auto notch = getNotch(increment);
	auto soundInc = getSoundIncrement(i);
	auto mtf = ls.lock()->lookupField(param).lock();
	
	if (mtf->isSplit())
	{
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}

	if (param.compare("st") == 0)
	{
		if (lengthFix && sound->getStart() + soundInc + oldLength > sound->getLastFrameIndex())
		{
			return;
		}
		
		sound->setStart(sound->getStart() + soundInc);
		
		if (lengthFix)
		{
			sound->setEnd(sound->getStart() + oldLength);
		}
	}
	else if (param.compare("end") == 0)
	{
		if (lengthFix && sound->getEnd() + soundInc - oldLength < 0)
		{
			return;
		}
		
		sound->setEnd(sound->getEnd() + soundInc);
		
		if (lengthFix)
		{
			sound->setStart(sound->getEnd() - oldLength);
		}
	}
	else if (param.compare("view") == 0)
	{
		soundGui->setView(soundGui->getView() + i);
	}
	else if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
	}
}

void TrimScreen::setSlider(int i)
{
	if (!Mpc::instance().getControls().lock()->isShiftPressed())
	{
		return;
	}
    
	init();

	auto zoomGui = mpc.getUis().lock()->getZoomGui();
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
    auto const lengthFix = zoomGui->isSmplLngthFix();
    auto candidatePos = (int) ((i / 124.0) * sound->getLastFrameIndex());
    auto maxPos = int (0);
	
	if (param.compare("st") == 0)
	{
		maxPos = lengthFix ? sound->getLastFrameIndex() - oldLength : sound->getLastFrameIndex();

		if (candidatePos > maxPos)
		{
			candidatePos = maxPos;
		}
		
		sound->setStart(candidatePos);
		
		if (lengthFix)
		{
			sound->setEnd(sound->getStart() + oldLength);
		}
	}
	else if (param.compare("end") == 0)
	{
		maxPos = lengthFix ? oldLength : int(0);
	
		if (candidatePos < maxPos)
		{
			candidatePos = maxPos;
		}
		
		sound->setEnd(candidatePos);
		
		if (lengthFix)
		{
			sound->setStart(sound->getEnd() - oldLength);
		}
	}
}

void TrimScreen::left()
{
	splitLeft();
}

void TrimScreen::right()
{
	splitRight();
}

void TrimScreen::pressEnter()
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

	auto zoomGui = mpc.getUis().lock()->getZoomGui(); 
	auto candidate = mtf->enter();
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
	auto const lengthFix = zoomGui->isSmplLngthFix();
	
	if (candidate != INT_MAX)
	{
		if (param.compare("st") == 0)
		{
			if (lengthFix && candidate + oldLength > sound->getLastFrameIndex())
			{
				return;
			}

			sound->setStart(candidate);
			
			if (lengthFix)
			{
				sound->setEnd(sound->getStart() + oldLength);
			}
		}
		else if (param.compare("end") == 0) {
			if (lengthFix && candidate - oldLength < 0)
			{
				return;
			}

			sound->setEnd(candidate);
			
			if (lengthFix)
			{
				sound->setStart(sound->getEnd() - oldLength);
			}
		}
	}
}
