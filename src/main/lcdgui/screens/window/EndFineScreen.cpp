#include "EndFineScreen.hpp"

#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

EndFineScreen::EndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "end-fine", layerIndex)
{
	addChildT<Wave>()->setFine(true);
}

void EndFineScreen::open()
{
    mpc.getControls()->getControls()->typableParams = { "end" };

    findField("smpllngth")->setAlignment(Alignment::Centered);
	findField("end")->enableTwoDots();
	displayEnd();
	displaySmplLngth();
	displayLngthLabel();
	findLabel("lngth")->enableTwoDots();
	displayPlayX();
	displayFineWave();
}

void EndFineScreen::displayFineWave()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sound = sampler->getSound();

	if (!sound)
		return;

	findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave()->setCenterSamplePos(sound->getEnd());
}


void EndFineScreen::displayEnd()
{
	auto sound = sampler->getSound();

	if (!sound)
		return;

	findField("end")->setTextPadded(sound->getEnd(), " ");
}

void EndFineScreen::displayLngthLabel()
{
	auto sound = sampler->getSound();

	if (!sound)
		return;

	findLabel("lngth")->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void EndFineScreen::displaySmplLngth()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
    findField("smpllngth")->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void EndFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void EndFineScreen::function(int i)
{
	ScreenComponent::function(i);

	switch (i)
	{
	case 1:
		findWave()->zoomPlus();
		break;
	case 2:
		findWave()->zoomMinus();
		break;
	case 4:
		sampler->playX();
		break;
	}
}

void EndFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler->getSound();

	auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

    auto soundInc = getSoundIncrement(i);
	auto field = findField(param);

	if (field->isSplit())
		soundInc = field->getSplitIncrement(i >= 0);

	if (field->isTypeModeEnabled())
		field->disableTypeMode();


	if (param == "end")
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
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	trimScreen->pressEnter();
	displayEnd();
	displayLngthLabel();
	displayFineWave();
}
