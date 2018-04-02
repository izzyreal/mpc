#include "FxEditControls.hpp"

#include <ui/sampler/MixerSetupGui.hpp>

#include <lcdgui/Effect.hpp>

using namespace mpc::controls::mixer;
using namespace std;

FxEditControls::FxEditControls(mpc::Mpc* mpc) 
	: AbstractMixerControls(mpc)
{
}

void FxEditControls::function(int f) {
	init();
	switch (f) {
	}
}

void FxEditControls::turnWheel(int i) {
	init();
	if (param.compare("drum") == 0) {
		mixerSetupGui->setFxDrum(mixerSetupGui->getFxDrum() + i);
	}
}

void FxEditControls::left() {
	super::left();
	checkEffects();
}

void FxEditControls::right() {
	super::right();
	checkEffects();
}

void FxEditControls::up() {
	super::up();
	checkEffects();
}

void FxEditControls::down() {
	super::down();
	checkEffects();
}

void FxEditControls::checkEffects() {
	string prevParam = param;
	init();
	if (prevParam.compare(param) != 0) {
		vector<string> effects{ "dist", "filt", "mod", "echo", "rev", "mix" };
		for (int i = 0; i < 6; i++) {
			auto effect = ls.lock()->getEffects()[i].lock();
			if (effects[i].compare(param) == 0)
				effect->setFilled(true);
			if (effects[i].compare(prevParam) == 0)
				effect->setFilled(false);
		}
	}
}

FxEditControls::~FxEditControls() {
}
