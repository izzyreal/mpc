#include "TrimScreen.hpp"

#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

TrimScreen::TrimScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "trim", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);

	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(false);

	baseControls->typableParams = { "st", "end" };
}

void TrimScreen::open()
{
	bool sound = sampler.lock()->getSound().lock() ? true : false;

	findField("snd").lock()->setFocusable(sound);
	findField("playx").lock()->setFocusable(sound);
	findField("st").lock()->setFocusable(sound);
	findField("st").lock()->enableTwoDots();
	findField("end").lock()->setFocusable(sound);
	findField("end").lock()->enableTwoDots();
	findField("view").lock()->setFocusable(sound);
	findField("dummy").lock()->setFocusable(!sound);

	displaySnd();
	displayPlayX();
	displaySt();
	displayEnd();
	displayView();
	displayWave();

	ls.lock()->setFunctionKeysArrangement(sound ? 1 : 0);
}

void TrimScreen::openWindow()
{
	init();
	
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("trim");
		ls.lock()->openScreen("sound");
	}
	else if (param.compare("st") == 0)
	{
		ls.lock()->openScreen("start-fine");
	}
	else if (param.compare("end") == 0)
	{
		ls.lock()->openScreen("end-fine");
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

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(mpc.screens->getScreenComponent("edit-sound"));
		editSoundScreen->setReturnToScreenName("trim");
		
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
	}
}

void TrimScreen::turnWheel(int i)
{
	init();

	if (param == "")
		return;

	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();
	
	if (field->isSplit())
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];
	
	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("st") == 0)
	{
		if (smplLngthFix && sound->getStart() + soundInc + oldLength > sound->getFrameCount())
			return;
		
		sound->setStart(sound->getStart() + soundInc);
		
		displaySt();

		if (smplLngthFix)
		{
			sound->setEnd(sound->getStart() + oldLength);
			displayEnd();
		}

		displayWave();
	}
	else if (param.compare("end") == 0)
	{
		if (smplLngthFix && sound->getEnd() + soundInc - oldLength < 0)
			return;
		
		sound->setEnd(sound->getEnd() + soundInc);
		
		displayEnd();

		if (smplLngthFix)
		{
			sound->setStart(sound->getEnd() - oldLength);
			displaySt();
		}
		displayWave();
	}
	else if (param.compare("view") == 0)
	{
		setView(view + i);
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->selectNextSound();
		displaySnd();
		displayEnd();
		displayPlayX();
		displaySt();
		displayView();
		displayWave();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->selectPreviousSound();
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
	if (!mpc.getControls().lock()->isShiftPressed())
		return;
    
	init();

	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
    auto candidatePos = (int) ((i / 124.0) * sound->getFrameCount());
    auto maxPos = int (0);
	
	if (param.compare("st") == 0)
	{
		maxPos = smplLngthFix ? sound->getFrameCount() - oldLength : sound->getFrameCount();

		if (candidatePos > maxPos)
			candidatePos = maxPos;
		
		sound->setStart(candidatePos);
		displaySt();

		if (smplLngthFix)
		{
			sound->setEnd(sound->getStart() + oldLength);
			displayEnd();
		}

		displayWave();
	}
	else if (param.compare("end") == 0)
	{
		maxPos = smplLngthFix ? oldLength : int(0);
	
		if (candidatePos < maxPos)
		{
			candidatePos = maxPos;
		}
		
		sound->setEnd(candidatePos);
		displayEnd();

		if (smplLngthFix)
		{
			sound->setStart(sound->getEnd() - oldLength);
			displaySt();
		}

		displayWave();
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
	
	auto field = findField(param).lock();
	
	if (!field->isTypeModeEnabled())
		return;

	auto candidate = field->enter();
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	if (candidate != INT_MAX)
	{
		if (param.compare("st") == 0)
		{
			if (smplLngthFix && candidate + oldLength > sound->getFrameCount())
				candidate = sound->getFrameCount() - oldLength;

			sound->setStart(candidate);
			displaySt();

			if (smplLngthFix)
			{
				sound->setEnd(sound->getStart() + oldLength);
				displayEnd();
			}

			displayWave();
		}
		else if (param.compare("end") == 0)
		{
			if (smplLngthFix && candidate - oldLength < 0)
				candidate = oldLength;

			sound->setEnd(candidate);
			displayEnd();
			
			if (smplLngthFix)
			{
				sound->setStart(sound->getEnd() - oldLength);
				displaySt();
			}

			displayWave();
		}
	}
}

void TrimScreen::displayWave()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findWave().lock()->setSampleData(nullptr, true, 0);
		findWave().lock()->setSelection(0, 0);
		return;
	}

	auto sampleData = sound->getSampleData();
	
	findWave().lock()->setSampleData(sampleData, sound->isMono(), view);
	findWave().lock()->setSelection(sound->getStart(), sound->getEnd());
}

void TrimScreen::displaySnd()
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


void TrimScreen::displayPlayX()
{
	findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void TrimScreen::displaySt()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		auto sound = sampler.lock()->getSound().lock();
		findField("st").lock()->setTextPadded(sound->getStart(), " ");
	}
	else {
		findField("st").lock()->setTextPadded("0", " ");
	}
}

void TrimScreen::displayEnd()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		auto sound = sampler.lock()->getSound().lock();
		findField("end").lock()->setTextPadded(sound->getEnd(), " ");
	}
	else {
		findField("end").lock()->setTextPadded("0", " ");
	}
}

void TrimScreen::displayView()
{
	if (view == 0)
	{
		findField("view").lock()->setText("LEFT");
	}
	else {
		findField("view").lock()->setText("RIGHT");
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
