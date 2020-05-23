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

	if (i >= 4)
	{
		return;
	}

	samplerGui->setSelectedDrum(i);
	auto previousScreenName = ls.lock()->getPreviousScreenName();
	auto name = string("programassign");

	if (previousScreenName.compare("programparams") == 0 || previousScreenName.compare("drum") == 0 || previousScreenName.compare("purge") == 0)
	{
		name = previousScreenName;
	}

	if (samplerGui->getNote() < 35)
	{
		samplerGui->setPadAndNote(program.lock()->getPadNumberFromNote(35), 35);
	}

	ls.lock()->openScreen(name);
	return;
}