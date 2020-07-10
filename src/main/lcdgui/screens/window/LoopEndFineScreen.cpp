#include "LoopEndFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopEndFineScreen::LoopEndFineScreen(const int layerIndex)
	: ScreenComponent("loop-end-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	addChild(move(make_shared<TwoDots>()));

	findWave().lock()->setFine(true);

	findTwoDots().lock()->setVisible(0, false);
	findTwoDots().lock()->setVisible(1, false);
	findTwoDots().lock()->setVisible(2, true);
	findTwoDots().lock()->setVisible(3, true);
	findTwoDots().lock()->setSelected(3, false);
}

void LoopEndFineScreen::open()
{
	displayEnd();
	displayLngthField();
	displayLoopLngth();

	displayPlayX();
	displayFineWaveform();
}

void LoopEndFineScreen::displayLoopLngth()
{
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(Screens::getScreenComponent("loop"));
	findField("loop-lngth").lock()->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopEndFineScreen::displayLngthField()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findField("lngth").lock()->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
}

void LoopEndFineScreen::displayFineWaveform()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));

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
	BaseControls::function(i);

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
	auto loopScreen = dynamic_pointer_cast<LoopScreen>(Screens::getScreenComponent("loop"));

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

		displayEnd();
		displayLngthField();
		displayFineWaveform();
	}
	else if (param.compare("end") == 0)
	{
		sound->setEnd(sound->getEnd() + i);

		if (loopScreen->loopLngthFix)
		{
			sound->setLoopTo(sound->getEnd() - loopLength);
		}

		displayEnd();
		displayLngthField();
		displayFineWaveform();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}
