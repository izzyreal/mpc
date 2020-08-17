#include "StartFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

StartFineScreen::StartFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "start-fine", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
	baseControls->typableParams = { "start" };

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
	displayFineWave();
}

void StartFineScreen::displayFineWave()
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

void StartFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sampleLength = sound->getFrameCount();
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = i >= 0 ? splitInc[field->getActiveSplit()] : -splitInc[field->getActiveSplit()];

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param.compare("start") == 0)
	{
		auto highestStartPos = sampleLength - 1;
		auto length = sound->getEnd() - sound->getStart();

		if (trimScreen->smplLngthFix)
		{
			highestStartPos -= startEndLength;

			if (sound->getStart() + soundInc > highestStartPos)
				return;
		}

		sound->setStart(sound->getStart() + soundInc);

		if (trimScreen->smplLngthFix)
			sound->setEnd(sound->getStart() + length);

		displayFineWave();
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

void StartFineScreen::left()
{
	splitLeft();
}

void StartFineScreen::right()
{
	splitRight();
}

void StartFineScreen::pressEnter()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
	trimScreen->pressEnter();
	displayStart();
	displayLngthLabel();
	displayFineWave();
}