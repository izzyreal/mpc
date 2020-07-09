#include "TrimScreen.hpp"

#include <lcdgui/screens/window/EditSoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

TrimScreen::TrimScreen(const int layerIndex)
	: ScreenComponent("trim", layerIndex)
{
	addChild(move(make_shared<TwoDots>()));
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(false);
}

void TrimScreen::open()
{
	typableParams = vector<string>{ "st", "end" };

	auto twoDots = findTwoDots().lock();
	twoDots->setVisible(0, true);
	twoDots->setVisible(1, true);
	twoDots->setVisible(2, false);
	twoDots->setVisible(3, false);

	if (!sampler.lock()->getSound().lock())
	{
		findField("snd").lock()->setFocusable(false);
		findField("playx").lock()->setFocusable(false);
		findField("end").lock()->setFocusable(false);
		findField("view").lock()->setFocusable(false);
		findField("st").lock()->setFocusable(false);
	}

	displaySnd();
	displayPlayX();
	displaySt();
	displayEnd();
	displayView();
	displayWave();
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

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(Screens::getScreenComponent("edit-sound"));
		editSoundScreen->setPreviousScreenName("trim");
		
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
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
	
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	//auto notch = getNotch(increment);
	auto soundInc = getSoundIncrement(i);
	auto mtf = findField(param).lock();
	
	if (mtf->isSplit())
	{
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}

	if (param.compare("st") == 0)
	{
		if (smplLngthFix && sound->getStart() + soundInc + oldLength > sound->getLastFrameIndex())
		{
			return;
		}
		
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
		{
			return;
		}
		
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
	if (!Mpc::instance().getControls().lock()->isShiftPressed())
	{
		return;
	}
    
	init();

	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getEnd() - sound->getStart();
    auto candidatePos = (int) ((i / 124.0) * sound->getLastFrameIndex());
    auto maxPos = int (0);
	
	if (param.compare("st") == 0)
	{
		maxPos = smplLngthFix ? sound->getLastFrameIndex() - oldLength : sound->getLastFrameIndex();

		if (candidatePos > maxPos)
		{
			candidatePos = maxPos;
		}
		
		sound->setStart(candidatePos);
		
		if (smplLngthFix)
		{
			sound->setEnd(sound->getStart() + oldLength);
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
		
		if (smplLngthFix)
		{
			sound->setStart(sound->getEnd() - oldLength);
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
	auto const oldLength = sound->getEnd() - sound->getStart();
	
	if (candidate != INT_MAX)
	{
		if (param.compare("st") == 0)
		{
			if (smplLngthFix && candidate + oldLength > sound->getLastFrameIndex())
			{
				return;
			}

			sound->setStart(candidate);
			
			if (smplLngthFix)
			{
				sound->setEnd(sound->getStart() + oldLength);
			}
			displayWave();
		}
		else if (param.compare("end") == 0)
		{
			if (smplLngthFix && candidate - oldLength < 0)
			{
				return;
			}

			sound->setEnd(candidate);
			
			if (smplLngthFix)
			{
				sound->setStart(sound->getEnd() - oldLength);
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
