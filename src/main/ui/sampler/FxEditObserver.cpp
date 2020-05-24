#include "FxEditObserver.hpp"

#include <Mpc.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Effect.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

using namespace mpc::ui::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

FxEditObserver::FxEditObserver()
{
	
	auto ls = Mpc::instance().getLayeredScreen().lock();

	drumField = ls->lookupField("drum");
	pgmField = ls->lookupField("pgm");
	editField = ls->lookupField("edit");

	ls->lookupField("dist").lock()->setText("DIST");
	ls->lookupField("filt").lock()->setText("FILT");
	ls->lookupField("mod").lock()->setText("MOD");
	ls->lookupField("echo").lock()->setText("ECHO");
	ls->lookupField("rev").lock()->setText("REV");
	ls->lookupField("mix").lock()->setText("MIX");
	displayDrum();
	displayPgm();
	displayEdit();

	effects = ls->getEffects();

	for (auto& e : effects) {
		e.lock()->Hide(false);
	}
}

void FxEditObserver::displayDrum()
{
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixersetup"));
	drumField.lock()->setText(to_string(mixerSetupScreen->getFxDrum() + 1));
}

void FxEditObserver::displayPgm()
{
	pgmField.lock()->setText("foo");
}

void FxEditObserver::displayEdit()
{
	editField.lock()->setText("foo");
}

void FxEditObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("fxdrum") == 0) {
		displayDrum();
	}
}

FxEditObserver::~FxEditObserver() {
	for (auto& e : effects) {
		e.lock()->Hide(true);
	}
}
