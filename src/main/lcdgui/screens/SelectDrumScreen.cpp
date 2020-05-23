#include <lcdgui/screens/SelectDrumScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Program.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

SelectDrumScreen::SelectDrumScreen(const int layerIndex) 
	: ScreenComponent("selectdrum", layerIndex)
{
}

void SelectDrumScreen::function(int i)
{
	init();
	auto lLs = ls.lock();
	if (i < 4) {
		samplerGui->setSelectedDrum(i);
		auto prevCsn = ls.lock()->getPreviousScreenName();
		auto name = string("programassign");
		if (prevCsn.compare("programparams") == 0 || prevCsn.compare("drum") == 0 || prevCsn.compare("purge") == 0)
			name = prevCsn;

		if (samplerGui->getNote() < 35)
			samplerGui->setPadAndNote(program.lock()->getPadNumberFromNote(35), 35);

		lLs->openScreen(name);
		return;
	}
}