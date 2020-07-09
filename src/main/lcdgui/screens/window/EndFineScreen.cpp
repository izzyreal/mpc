#include "EndFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

EndFineScreen::EndFineScreen(const int layerIndex)
	: ScreenComponent("end-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	addChild(move(make_shared<TwoDots>()));
	findWave().lock()->setFine(true);

	findTwoDots().lock()->setVisible(0, false);
	findTwoDots().lock()->setVisible(1, false);
	findTwoDots().lock()->setVisible(2, true);
	findTwoDots().lock()->setVisible(3, true);
}

void EndFineScreen::open()
{
	displayEnd();
	displayLngthLabel();
	displaySmplLngth();

	displayPlayX();
	displayFineWaveform();
	findTwoDots().lock()->setSelected(3, false);
}

void EndFineScreen::displayFineWaveform()
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


void EndFineScreen::displayEnd()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findField("end").lock()->setTextPadded(sound->getEnd(), " ");
}

void EndFineScreen::displayLngthLabel()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		return;
	}

	findLabel("lngth").lock()->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void EndFineScreen::displaySmplLngth()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));
    findField("smpllngth").lock()->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void EndFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void EndFineScreen::function(int i)
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

void EndFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();

	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(Screens::getScreenComponent("trim"));
	auto sampleLength = sound->getFrameCount();

	if (param.compare("end") == 0)
	{
		sound->setEnd(sound->getEnd() + i);

		if (trimScreen->smplLngthFix)
		{
			sound->setStart(sound->getEnd() - startEndLength);
		}

		displayLngthLabel();
		displayEnd();
		displayFineWaveform();
	}
	else if (param.compare("smpllngth") == 0)
	{
		trimScreen->smplLngthFix = i > 0;
		displaySmplLngth();
	}
	else if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
}
