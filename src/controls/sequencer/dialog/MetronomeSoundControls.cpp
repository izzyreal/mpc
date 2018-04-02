#include "MetronomeSoundControls.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

MetronomeSoundControls::MetronomeSoundControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void MetronomeSoundControls::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("countmetronome");
		break;
	}
}

void MetronomeSoundControls::turnWheel(int i)
{
    init();
	if (param.compare("sound") == 0) {
		swGui->setMetronomeSound(swGui->getMetronomeSound() + i);
	}
	else if (param.compare("volume") == 0) {
		swGui->setClickVolume(swGui->getClickVolume() + i);
	}
	else if (param.compare("output") == 0) {
		swGui->setClickOutput(swGui->getClickOutput() + i);
	}
	else if (param.compare("accent") == 0) {
		swGui->setAccentNote(swGui->getAccentNote() + i);
	}
	else if (param.compare("normal") == 0) {
		swGui->setNormalNote(swGui->getNormalNote() + i);
	}
	else if (param.compare("velocityaccent") == 0) {
		swGui->setAccentVelo(swGui->getAccentVelo() + i);
	}
	else if (param.compare("velocitynormal") == 0) {
		swGui->setNormalVelo(swGui->getNormalVelo() + i);
	}
}
