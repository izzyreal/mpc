#include "LoopToFineScreen.hpp"

#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

LoopToFineScreen::LoopToFineScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-to-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void LoopToFineScreen::open()
{
    mpc.getControls()->getBaseControls()->typableParams = { "to", "lngth" };

    findField("loop-lngth")->setAlignment(Alignment::Centered);
	displayTo();
	findField("to")->enableTwoDots();
	displayLngthField();
	displayLoopLngth();
	findField("lngth")->enableTwoDots();

	displayPlayX();
	displayFineWave();
}

void LoopToFineScreen::displayLoopLngth()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	findField("loop-lngth")->setText(loopScreen->loopLngthFix ? "FIX" : "VARI");
}

void LoopToFineScreen::displayLngthField()
{
	auto sound = sampler->getSound();
	
	if (!sound)
	{
		return;
	}

	findField("lngth")->setTextPadded(sound->getEnd() - sound->getLoopTo(), " ");
}

void LoopToFineScreen::displayFineWave()
{
	auto trimScreen = mpc.screens->get<TrimScreen>("trim");
	auto sound = sampler->getSound();
	
	if (!sound)
	{
		return;
	}

	findWave()->setSampleData(sound->getSampleData(), sound->isMono(), trimScreen->view);
	findWave()->setCenterSamplePos(sound->getLoopTo());
}

void LoopToFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void LoopToFineScreen::displayTo()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		return;
	}

	findField("to")->setTextPadded(sound->getLoopTo(), " ");
}

void LoopToFineScreen::function(int i)
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

void LoopToFineScreen::turnWheel(int i)
{
	init();
	auto sound = sampler->getSound();
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");

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

        displayTo();
		displayLngthField();
		displayFineWave();
	}
	else if (param == "to")
	{
        loopScreen->setLoopTo(sound->getLoopTo() + soundInc);

		displayTo();
		displayLngthField();
		displayFineWave();
	}
	else if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
}

void LoopToFineScreen::left()
{
	splitLeft();
}

void LoopToFineScreen::right()
{
	splitRight();
}

void LoopToFineScreen::pressEnter()
{
	auto loopScreen = mpc.screens->get<LoopScreen>("loop");
	loopScreen->pressEnter();
	displayTo();
	displayLngthField();
	displayFineWave();
}

void LoopToFineScreen::setSlider(int i)
{
    if (!mpc.getControls()->isShiftPressed())
    {
        return;
    }

    init();

    auto loopScreen = mpc.screens->get<LoopScreen>("loop");

    if (param == "to")
    {
        loopScreen->setSliderLoopTo(i);
        displayTo();
        displayLngthField();
        displayFineWave();
    }
    else if (param == "lngth")
    {
        loopScreen->setSliderLength(i);
        displayTo();
        displayLngthField();
        displayFineWave();
    }
}
