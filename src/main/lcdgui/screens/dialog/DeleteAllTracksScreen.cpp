#include "DeleteAllTracksScreen.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllTracksScreen::DeleteAllTracksScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-tracks", layerIndex)
{
}

void DeleteAllTracksScreen::function(int i)
{
	init();
	switch (i)
	{
	case 3:
		openScreen("delete-track");
		break;
	case 4:
		sequencer.lock()->getActiveSequence().lock()->purgeAllTracks();
		openScreen("sequencer");
		break;
	}
}
