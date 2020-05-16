#include <lcdgui/screens/window/MidiInputScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

MidiInputScreen::MidiInputScreen(const int& layer)
	: ScreenComponent("midiinput", layer)
{
}

void MidiInputScreen::function(int i)
{
	BaseControls::function(i);
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("midiinputmonitor");
		break;
	}
}

void MidiInputScreen::turnWheel(int i)
{
	init();

	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("receivech") == 0)
	{
		swGui->setReceiveCh(swGui->getReceiveCh() + i);
	}
	else if (param.compare("seq") == 0)
	{
		swGui->setProgChangeSeq(i > 0);
	}
	else if (param.compare("duration") == 0)
	{
		swGui->setSustainPedalToDuration(i > 0);
	}
	else if (param.compare("midifilter") == 0)
	{
		swGui->setMidiFilterEnabled(i > 0);
	}
	else if (param.compare("type") == 0)
	{
		swGui->setFilterType(swGui->getMidiFilterType() + i);
	}
	else if (param.compare("pass") == 0)
	{
		swGui->setPass(i > 0);
	}
}
