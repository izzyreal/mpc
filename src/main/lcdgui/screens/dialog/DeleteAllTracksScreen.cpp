#include "DeleteAllTracksScreen.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllTracksScreen::DeleteAllTracksScreen(const int layerIndex)
	: ScreenComponent("deletealltracks", layerIndex)
{
}

void DeleteAllTracksScreen::function(int i)
{
	init();
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("deletetrack");
		break;
	case 4:
		sequencer.lock()->getActiveSequence().lock()->purgeAllTracks();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
