#include "LoopToFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopToFineScreen::LoopToFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-to-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void LoopToFineScreen::open()
{
    mpc.getControls().lock()->getControls()->typableParams = { "to", "lngth" };

    findField("loop-lngth").lock()->setAlignment(Alignment::Centered);
	displayTo();
	findField("to").lock()->enableTwoDots();
	displayLngthField();
	displayLoopLngth();
	findField("lngth").lock()->enableTwoDots();

	displayPlayX();
	displayFineWave();
}

void LoopToFineScreen::displayLoopLngth()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	findField("loop-lngth").lock()->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopToFineScreen::displayLngthField()
{
	auto sound = sampler.lock()->getSound().lock();
	
	if (!sound)
	{
		return;
	}

	findField("lngth").lock()->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
}

void LoopToFineScreen::displayFineWave()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	auto sound = sampler.lock()->getSound().lock();
	
	if (!sound)
	{
		return;
	}

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(sound->getLoopTo());
}

void LoopToFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void LoopToFineScreen::displayTo()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findField("to").lock()->setTextPadded(sound->getLoopTo(), " ");
}

void LoopToFineScreen::function(int i)
{

	ScreenComponent::function(i);

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

void LoopToFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto loopLength = static_cast<int>(sound->getEnd() - sound->getLoopTo());
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

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
		
		displayLngthField();
		displayFineWave();
	}
	else if (param.compare("to") == 0)
	{
		if (loopScreen->loopLngthFix)
		{
			int highestLoopTo = sound->getFrameCount() - loopLength;

			if (sound->getLoopTo() + soundInc > highestLoopTo)
				soundInc = sound->getLoopTo() - highestLoopTo;
		}

		sound->setLoopTo(sound->getLoopTo() + soundInc);

		if (loopScreen->loopLngthFix)
			sound->setEnd(sound->getLoopTo() + loopLength);

		displayTo();
		displayLngthField();
		displayFineWave();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}

void LoopToFineScreen::left()
{
	splitLeft();
}

void LoopToFineScreen::right()
{
	splitRight();
}

void LoopToFineScreen::pressEnter()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	loopScreen->pressEnter();
	displayTo();
	displayLngthField();
	displayFineWave();
}
