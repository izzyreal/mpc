#include "FxEditScreen.hpp"

#include <lcdgui/Effect.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

FxEditScreen::FxEditScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "fx-edit", layerIndex)
{
	int effectWidth = 30;
	int effectHeight = 14;
	int effectDistance = 5;
	int effectOffset = 42;
	//int effectY = 36; // when effect is off
	int effectY = 23; // when effect is on

	for (int i = 0; i < 6; i++)
	{
		int x = (i * (effectWidth + effectDistance)) + effectOffset;
		MRECT r(x, effectY, x + effectWidth - 1, effectY + effectHeight - 1);
		addChildT<Effect>(r);
	}

}

void FxEditScreen::open()
{
	displayDrum();
	displayPgm();
	displayEdit();
}

void FxEditScreen::function(int f) {
	init();
	switch (f) {
	}
}

void FxEditScreen::turnWheel(int i)
{
	init();
	if (param == "drum")
	{
		auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
		mixerSetupScreen->setFxDrum(mixerSetupScreen->getFxDrum() + i);
	}
}

void FxEditScreen::left()
{
	ScreenComponent::left();
	checkEffects();
}

void FxEditScreen::right()
{
	ScreenComponent::right();
	checkEffects();
}

void FxEditScreen::up()
{
	ScreenComponent::up();
	checkEffects();
}

void FxEditScreen::down()
{
	ScreenComponent::down();
	checkEffects();
}

std::vector<std::weak_ptr<Effect>> FxEditScreen::findEffects()
{
	std::vector<std::weak_ptr<Effect>> result;

	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Effect>(c);
		if (candidate)
		{
			result.push_back(candidate);
		}
	}

	return result;
}


void FxEditScreen::checkEffects() {
    std::string prevParam = param;

	init();

	if (prevParam != param)
	{
        std::vector<std::string> effects{ "dist", "filt", "mod", "echo", "rev", "mix" };
		for (int i = 0; i < 6; i++)
		{
			auto effect = findEffects()[i].lock();
		
			if (effects[i] == param)
			{
				effect->setFilled(true);
			}

			if (effects[i] == prevParam)
			{
				effect->setFilled(false);
			}
		}
	}
}

void FxEditScreen::displayDrum()
{
	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
	findField("drum")->setText(std::to_string(mixerSetupScreen->getFxDrum() + 1));
}

void FxEditScreen::displayPgm()
{
	findField("pgm")->setText("foo");
}

void FxEditScreen::displayEdit()
{
	findField("edit")->setText("foo");
}
