#include "FxEditScreen.hpp"

#include <lcdgui/Effect.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

FxEditScreen::FxEditScreen(const int layerIndex) 
	: ScreenComponent("fx-edit", layerIndex)
{
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

void FxEditScreen::turnWheel(int i) {
	init();
	if (param.compare("drum") == 0)
	{
		auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
		mixerSetupScreen->setFxDrum(mixerSetupScreen->getFxDrum() + i);
	}
}

void FxEditScreen::left()
{
	BaseControls::left();
	checkEffects();
}

void FxEditScreen::right() {
	BaseControls::right();
	checkEffects();
}

void FxEditScreen::up() {
	BaseControls::up();
	checkEffects();
}

void FxEditScreen::down() {
	BaseControls::down();
	checkEffects();
}

void FxEditScreen::checkEffects() {
	string prevParam = param;

	init();

	if (prevParam.compare(param) != 0)
	{
		vector<string> effects{ "dist", "filt", "mod", "echo", "rev", "mix" };
		for (int i = 0; i < 6; i++) {
			auto effect = ls.lock()->getEffects()[i].lock();
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
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
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
