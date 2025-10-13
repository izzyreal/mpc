#include "EndFineScreen.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"

#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/LoopScreen.hpp>

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

    auto soundInc = getSoundIncrement(i);
	auto field = findField(param);

	if (field->isSplit())
    {
        soundInc = field->getSplitIncrement(i >= 0);
    }

	if (field->isTypeModeEnabled())
    {
        field->disableTypeMode();
    }

    auto trimScreen = mpc.screens->get<TrimScreen>("trim");

    if (param == "end")
	{
		auto newValue = sound->getEnd() + soundInc;
        trimScreen->setEnd(newValue);

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
    command::SplitLeftCommand(mpc).execute();
}

void EndFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void EndFineScreen::pressEnter()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	trimScreen->pressEnter();
	displayEnd();
	displayLngthLabel();
	displayFineWave();
}

void EndFineScreen::setSlider(int i)
{
    if (!mpc.getHardware2()->getButton("shift")->isPressed())
    {
        return;
    }

    init();

    if (param == "end")
    {
        auto trimScreen = mpc.screens->get<TrimScreen>("trim");
        trimScreen->setSliderEnd(i);
        displayEnd();
        displayLngthLabel();
        displayFineWave();
    }
}
