#include "StartFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

StartFineScreen::StartFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "start-fine", layerIndex)
{
	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void StartFineScreen::open()
{
	displayStart();
	findField("start").lock()->enableTwoDots();
	displayLngthLabel();
	findLabel("lngth").lock()->enableTwoDots();
	displaySmplLngth();
	displayPlayX();
	displayFineWaveform();
}

void StartFineScreen::displayFineWaveform()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(sound->getStart());
}

void StartFineScreen::displayStart()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findField("start").lock()->setTextPadded(sound->getStart(), " ");
}

void StartFineScreen::displayLngthLabel()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findLabel("lngth").lock()->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void StartFineScreen::displaySmplLngth()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
    findField("smpllngth").lock()->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void StartFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void StartFineScreen::function(int i)
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

void StartFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sampleLength = sound->getFrameCount();

	if (param.compare("start") == 0)
	{
		auto highestStartPos = sampleLength - 1;
		auto length = sound->getEnd() - sound->getStart();

		if (trimScreen->smplLngthFix)
		{
			highestStartPos -= startEndLength;

			if (sound->getStart() + i > highestStartPos)
			{
				return;
			}
		}

		sound->setStart(sound->getStart() + i);

		if (trimScreen->smplLngthFix)
		{
			sound->setEnd(sound->getStart() + length);
		}

		displayFineWaveform();
		displayLngthLabel();
		displayStart();
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
