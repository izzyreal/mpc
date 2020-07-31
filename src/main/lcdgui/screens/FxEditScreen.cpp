#include "FxEditScreen.hpp"

#include <lcdgui/Effect.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

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
		addChild(make_shared<Effect>(r));
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
	if (param.compare("drum") == 0)
	{
		auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
		mixerSetupScreen->setFxDrum(mixerSetupScreen->getFxDrum() + i);
	}
}

void FxEditScreen::left()
{
	baseControls->left();
	checkEffects();
}

void FxEditScreen::right()
{
	baseControls->right();
	checkEffects();
}

void FxEditScreen::up()
{
	baseControls->up();
	checkEffects();
}

void FxEditScreen::down()
{
	baseControls->down();
	checkEffects();
}

vector<weak_ptr<Effect>> FxEditScreen::findEffects()
{
	vector<weak_ptr<Effect>> result;

	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Effect>(c);
		if (candidate)
		{
			result.push_back(candidate);
		}
	}

	return result;
}


void FxEditScreen::checkEffects() {
	string prevParam = param;

	init();

	if (prevParam.compare(param) != 0)
	{
		vector<string> effects{ "dist", "filt", "mod", "echo", "rev", "mix" };
		for (int i = 0; i < 6; i++)
		{
			auto effect = findEffects()[i].lock();
		
			if (effects[i].compare(param) == 0)
			{
				effect->setFilled(true);
			}

			if (effects[i].compare(prevParam) == 0)
			{
				effect->setFilled(false);
			}
		}
	}
}

void FxEditScreen::displayDrum()
{
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	findField("drum").lock()->setText(to_string(mixerSetupScreen->getFxDrum() + 1));
}

void FxEditScreen::displayPgm()
{
	findField("pgm").lock()->setText("foo");
}

void FxEditScreen::displayEdit()
{
	findField("edit").lock()->setText("foo");
}
