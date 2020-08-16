#include "LoopEndFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopEndFineScreen::LoopEndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-end-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void LoopEndFineScreen::open()
{
	findField("end").lock()->enableTwoDots();
	displayEnd();
	displayLngthField();
	findField("lngth").lock()->enableTwoDots();
	displayLoopLngth();

	displayPlayX();
	displayFineWave();
}

void LoopEndFineScreen::displayLoopLngth()
{
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));
	findField("loop-lngth").lock()->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopEndFineScreen::displayLngthField()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findField("lngth").lock()->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
}

void LoopEndFineScreen::displayFineWave()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(sound->getEnd());
}

void LoopEndFineScreen::displayEnd()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findField("end").lock()->setTextPadded(sound->getEnd(), " ");
}

void LoopEndFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void LoopEndFineScreen::function(int i)
{
	baseControls->function(i);

	switch (i)
	{
	case 1:
		findWave().lock()->zoomPlus();
		break;
	case 2:
		findWave().lock()->zoomMinus();
		break;
	case 4:
		sampler.lock()->playX();
		break;
	}
}

void LoopEndFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto loopLength = static_cast<int>((sound->getEnd() - sound->getLoopTo()));
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));

	auto sampleLength = sound->getFrameCount();

	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("loop-lngth") == 0)
	{
		loopScreen->loopLngthFix = i > 0;
		displayLoopLngth();
	}
	else if (param.compare("lngth") == 0)
	{		
		sound->setEnd(sound->getEnd() + soundInc);

		displayEnd();
		displayLngthField();
		displayFineWave();
	}
	else if (param.compare("end") == 0)
	{
		sound->setEnd(sound->getEnd() + soundInc);

		if (loopScreen->loopLngthFix)
			sound->setLoopTo(sound->getEnd() - loopLength);

		displayEnd();
		displayLngthField();
		displayFineWave();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}

void LoopEndFineScreen::left()
{
	splitLeft();
}

void LoopEndFineScreen::right()
{
	splitRight();
}

void LoopEndFineScreen::pressEnter()
{
	init();

	auto field = findField(param).lock();

	if (!field->isTypeModeEnabled())
		return;

	auto candidate = field->enter();
	auto sound = sampler.lock()->getSound().lock();
	auto const oldLength = sound->getLoopTo() - sound->getStart();
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));
	auto loopLngthFix = loopScreen->loopLngthFix;

	if (candidate != INT_MAX)
	{
		if (param.compare("end") == 0)
		{
			if (loopLngthFix && candidate - oldLength < 0)
				return;

			sound->setEnd(candidate);
			displayEnd();

			if (loopLngthFix)
				sound->setLoopTo(sound->getEnd() - oldLength);
			
			displayLngthField();
			displayFineWave();
		}
		else if (param.compare("lngth") == 0)
		{
			sound->setEnd(candidate);
			displayLngthField();
		}
	}
}
