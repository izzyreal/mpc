#include "MidiOutputScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

MidiOutputScreen::MidiOutputScreen(const int& layer)
	: ScreenComponent("midioutput", layer)
{
}

void MidiOutputScreen::turnWheel(int i)
{
	init();
	
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
	if (param.compare("firstletter") == 0)
	{
		nameGui->setName(seq->getDeviceName(swGui->getDeviceNumber() + i));
		nameGui->setParameterName("devicename");
		nameGui->setNameLimit(8);
		ls.lock()->openScreen("name");
	}
	else if (param.compare("softthru") == 0)
	{
		swGui->setSoftThru(swGui->getSoftThru() + i);
	}
	else if (param.compare("devicenumber") == 0)
	{
		swGui->setDeviceNumber(swGui->getDeviceNumber() + i);
	}
}

void MidiOutputScreen::function(int i)
{
	BaseControls::function(i);
	
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("midioutputmonitor");
		break;
	}
}
