#include "LoopToFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopToFineScreen::LoopToFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-to-fine", layerIndex)
{
}

void LoopToFineScreen::open()
{
	addChild(move(make_shared<Wave>()));
	addChild(move(make_shared<TwoDots>()));

	displayTo();
	displayLngthField();
	displayLoopLngth();

	displayPlayX();
	displayFineWaveform();

	findTwoDots().lock()->setVisible(0, false);
	findTwoDots().lock()->setVisible(1, false);
	findTwoDots().lock()->setVisible(2, true);
	findTwoDots().lock()->setVisible(3, true);
	findTwoDots().lock()->setSelected(3, false);
}

void LoopToFineScreen::displayLoopLngth()
{
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));
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

void LoopToFineScreen::displayFineWaveform()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
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

void LoopToFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto loopLength = static_cast<int>((sound->getEnd() - sound->getLoopTo()));
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(mpc.screens->getScreenComponent("loop"));

	auto sampleLength = sound->getFrameCount();
	
	if (param.compare("loop-lngth") == 0)
	{
		loopScreen->loopLngthFix = i > 0;
		displayLoopLngth();
	}
	else if (param.compare("lngth") == 0)
	{
		if (sound->getEnd() + i > sampleLength)
		{
			return;
		}
		
		sound->setEnd(sound->getEnd() + i);
		
		displayLngthField();
		displayFineWaveform();
	}
	else if (param.compare("to") == 0)
	{
		if (!loopScreen->loopLngthFix && sound->getEnd() - (sound->getLoopTo() + i) < 0)
		{
			return;
		}

		auto highestLoopTo = sampleLength - 1;

		if (loopScreen->loopLngthFix)
		{
			highestLoopTo -= loopLength;

			if (sound->getLoopTo() + i > highestLoopTo)
			{
				return;
			}
		}

		sound->setLoopTo(sound->getLoopTo() + i);

		if (loopScreen->loopLngthFix)
		{
			sound->setEnd(sound->getLoopTo() + loopLength);
		}

		displayTo();
		displayLngthField();
		displayFineWaveform();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}
