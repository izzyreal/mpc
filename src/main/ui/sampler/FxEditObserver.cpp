#include "FxEditObserver.hpp"

#include <Mpc.hpp>

#include <ui/Uis.hpp>
#include <ui/sampler/MixerSetupGui.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Effect.hpp>

using namespace mpc::ui::sampler;
using namespace std;

FxEditObserver::FxEditObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	auto ls = mpc->getLayeredScreen().lock();

	msGui = mpc->getUis().lock()->getMixerSetupGui();
	msGui->addObserver(this);

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
	drumField.lock()->setText(to_string(msGui->getFxDrum() + 1));
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
	msGui->deleteObserver(this);
	for (auto& e : effects) {
		e.lock()->Hide(true);
	}
}
