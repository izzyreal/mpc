#include "StartFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

StartFineScreen::StartFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "start-fine", layerIndex)
{
	addChild(std::move(std::make_shared<Wave>()));
	findWave().lock()->setFine(true);
}

void StartFineScreen::open()
{
    mpc.getControls().lock()->getControls()->typableParams = { "start" };

	findField("smpllngth").lock()->setAlignment(Alignment::Centered);
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
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sound = sampler->getSound().lock();

	if (!sound)
		return;

	findWave().lock()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave().lock()->setCenterSamplePos(sound->getStart());
}

void StartFineScreen::displayStart()
{
	auto sound = sampler->getSound().lock();

	if (!sound)
		return;

	findField("start").lock()->setTextPadded(sound->getStart(), " ");
}

void StartFineScreen::displayLngthLabel()
{
	auto sound = sampler->getSound().lock();

	if (!sound)
		return;

	findLabel("lngth").lock()->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void StartFineScreen::displaySmplLngth()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
    findField("smpllngth").lock()->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void StartFineScreen::displayPlayX()
{
    findField("playx").lock()->setText(playXNames[sampler->getPlayX()]);
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
		sampler->playX();
		break;
	}
}

void StartFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler->getSound().lock();
	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sampleLength = sound->getFrameCount();
	auto soundInc = getSoundIncrement(i);
	auto field = findField(param).lock();

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();

	if (param == "start")
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
	else if (param == "smpllngth")
	{
		trimScreen->smplLngthFix = i > 0;
		displaySmplLngth();
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
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
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	trimScreen->pressEnter();
	displayStart();
	displayLngthLabel();
	displayFineWave();
}
