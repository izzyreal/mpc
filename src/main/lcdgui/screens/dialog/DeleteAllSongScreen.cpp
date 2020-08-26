#include "DeleteAllSongScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllSongScreen::DeleteAllSongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-song", layerIndex)
{
}

void DeleteAllSongScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("delete-song");
		break;
	case 4:
		
		for (int j = 0; j < 20; j++)
			sequencer.lock()->deleteSong(j);

		openScreen("sequencer");
		break;
	}
}
