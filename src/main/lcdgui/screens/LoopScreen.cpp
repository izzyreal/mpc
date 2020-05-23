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
using namespace moduru::lang;
using namespace std;

LoopScreen::LoopScreen(const int layerIndex)
	: ScreenComponent("loop", layerIndex)
{
}

void LoopScreen::open()
{
	typableParams = vector<string>{ "to", "endlengthvalue" };

	addChild(move(make_shared<TwoDots>()));
	addChild(move(make_shared<Wave>()));

	auto twoDots = findTwoDots().lock();
	twoDots->setVisible(0, true);
	twoDots->setVisible(1, true);
	twoDots->setVisible(2, false);
	twoDots->setVisible(3, false);

	if (sampler.lock()->getSoundCount() != 0)
	{
		findField("dummy").lock()->setFocusable(false);
		waveformLoadData();
		
		auto sound = sampler.lock()->getSound().lock();
		findWave().lock()->setSelection(sound->getLoopTo(), sound->getEnd());
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->initZones(sampler.lock()->getSound().lock()->getFrameCount());
	}
	else
	{
		findWave().lock()->setSampleData(nullptr, false, 0);
		findField("snd").lock()->setFocusable(false);
		findField("playx").lock()->setFocusable(false);
		findField("to").lock()->setFocusable(false);
		findField("endlength").lock()->setFocusable(false);
		findField("endlengthvalue").lock()->setFocusable(false);
		findField("loop").lock()->setFocusable(false);
	}
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

		displayEndLength();
		displayEndLengthValue();
		displayTo();
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
    }
	else if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("loop") == 0)
	{
		sound->setLoopEnabled(i > 0);
		displayLoop();
	}
	else if (param.compare("endlength") == 0)
	{
		soundGui->setEndSelected(i > 0);
		displayEndLength();
		displayEndLengthValue();

	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
		displaySnd();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
		displaySnd();
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

		displayEndLength();
		displayEndLengthValue();
		displayTo();
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

void LoopScreen::displaySnd()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		if (ls.lock()->getFocus().compare("dummy") == 0)
		{
			ls.lock()->setFocus(findField("snd").lock()->getName());
		}

		auto sound = sampler.lock()->getSound().lock();

		auto sampleName = sound->getName();

		if (!sound->isMono())
		{
			sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";
		}
		findField("snd").lock()->setText(sampleName);
	}
	else {
		findField("snd").lock()->setText("(no sound)");
		ls.lock()->setFocus("dummy");
	}
}

void LoopScreen::displayPlayX()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("playx").lock()->setText(playXNames[soundGui->getPlayX()]);
}

void LoopScreen::displayTo()
{
	if (sampler.lock()->getSoundCount() != 0) {
		auto sound = sampler.lock()->getSound().lock();
		findField("to").lock()->setTextPadded(sound->getLoopTo(), " ");
	}
	else {
		findField("to").lock()->setTextPadded("0", " ");
	}

	auto soundGui = mpc.getUis().lock()->getSoundGui();

	if (!soundGui->isEndSelected())
	{
		displayEndLengthValue();
	}
}

void LoopScreen::displayEndLength()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("endlength").lock()->setText(soundGui->isEndSelected() ? "  End" : "Lngth");
}

void LoopScreen::displayEndLengthValue()
{
	if (sampler.lock()->getSoundCount() == 0)
	{
		findField("endlengthvalue").lock()->setTextPadded("0", " ");
		return;
	}

	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto sound = sampler.lock()->getSound().lock();

	if (soundGui->isEndSelected())
	{
		findField("endlengthvalue").lock()->setTextPadded(sound->getEnd(), " ");
	}
	else
	{
		findField("endlengthvalue").lock()->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
	}
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

void LoopScreen::waveformLoadData()
{
	auto sampleData = sampler.lock()->getSound().lock()->getSampleData();
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findWave().lock()->setSampleData(sampleData, sampler.lock()->getSound().lock()->isMono(), soundGui->getView());
}
