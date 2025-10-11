#include "LoopEndFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

LoopEndFineScreen::LoopEndFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-end-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void LoopEndFineScreen::open()
{
    findField("loop-lngth")->setAlignment(Alignment::Centered);
	findField("end")->enableTwoDots();
	displayEnd();
	displayLngthField();
	findField("lngth")->enableTwoDots();
	displayLoopLngth();

	displayPlayX();
	displayFineWave();
}

void LoopEndFineScreen::displayLoopLngth()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	findField("loop-lngth")->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopEndFineScreen::displayLngthField()
{
	auto sound = sampler->getSound();

	if (!sound)
		return;

	findField("lngth")->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
}

void LoopEndFineScreen::displayFineWave()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");

	auto sound = sampler->getSound();

	if (!sound)
	{
		return;
	}

	findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave()->setCenterSamplePos(sound->getEnd());
}

void LoopEndFineScreen::displayEnd()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		return;
	}

	findField("end")->setTextPadded(sound->getEnd(), " ");
}

void LoopEndFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void LoopEndFineScreen::function(int i)
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

void LoopEndFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler->getSound();
    auto loopScreen = mpc.screens->get<LoopScreen>("loop");
    auto trimScreen = mpc.screens->get<TrimScreen>("trim");

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

	if (param == "loop-lngth")
	{
		loopScreen->loopLngthFix = i > 0;
		displayLoopLngth();
	}
	else if (param == "lngth")
	{
        auto newLength = (sound->getEnd() - sound->getLoopTo()) + soundInc;

        loopScreen->setLength(newLength);
		displayEnd();
		displayLngthField();
		displayFineWave();
	}
	else if (param == "end")
	{
		auto newValue = sound->getEnd() + soundInc;
        trimScreen->setEnd(newValue);

		displayEnd();
		displayLngthField();
		displayFineWave();
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
}

void LoopEndFineScreen::left()
{
	splitLeft();
}

void LoopEndFineScreen::right()
{
	splitRight();
}

void LoopEndFineScreen::pressEnter()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	loopScreen->pressEnter();
	displayEnd();
	displayLngthField();
	displayFineWave();
}

void LoopEndFineScreen::setSlider(int i)
{
    if (!mpc.getControls()->isShiftPressed())
    {
        return;
    }

    init();

    auto loopScreen = mpc.screens->get<LoopScreen>("loop");
    auto trimScreen = mpc.screens->get<TrimScreen>("trim");

    if (param == "end")
    {
        trimScreen->setSliderEnd(i);
        displayEnd();
        displayLngthField();
        displayFineWave();
    }
    else if (param == "lngth")
    {
        loopScreen->setSliderLength(i);
        displayEnd();
        displayLngthField();
        displayFineWave();
    }
}
