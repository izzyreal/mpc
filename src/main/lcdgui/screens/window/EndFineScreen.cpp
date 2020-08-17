#include "EndFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

EndFineScreen::EndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "end-fine", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
	baseControls->typableParams = { "end" };

	addChild(move(make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void EndFineScreen::open()
{
	findField("end").lock()->enableTwoDots();
	displayEnd();
	displaySmplLngth();
	displayLngthLabel();
	findLabel("lngth").lock()->enableTwoDots();
	displayPlayX();
	displayFineWave();
}

void EndFineScreen::displayFineWave()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));

	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(sound->getEnd());
}


void EndFineScreen::displayEnd()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findField("end").lock()->setTextPadded(sound->getEnd(), " ");
}

void EndFineScreen::displayLngthLabel()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
		return;

	findLabel("lngth").lock()->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void EndFineScreen::displaySmplLngth()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
    findField("smpllngth").lock()->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void EndFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}

void EndFineScreen::function(int i)
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

void EndFineScreen::turnWheel(int i)
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


	if (param.compare("end") == 0)
	{
		auto candidate = sound->getEnd() + soundInc;

		if (trimScreen->smplLngthFix && candidate < startEndLength)
			candidate = startEndLength;

		sound->setEnd(candidate);

		if (trimScreen->smplLngthFix)
			sound->setStart(sound->getEnd() - startEndLength);

		displayLngthLabel();
		displayEnd();
		displayFineWave();
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

void EndFineScreen::left()
{
	splitLeft();
}

void EndFineScreen::right()
{
	splitRight();
}

void EndFineScreen::pressEnter()
{
	auto trimScreen = dynamic_pointer_cast<TrimScreen>(mpc.screens->getScreenComponent("trim"));
	trimScreen->pressEnter();
	displayEnd();
	displayLngthLabel();
	displayFineWave();
}