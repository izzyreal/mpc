#include "DeleteAllSequencesScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllSequencesScreen::DeleteAllSequencesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-sequences", layerIndex)
{
}

void DeleteAllSequencesScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("delete-sequence");
		break;
	case 4:
		sequencer.lock()->purgeAllSequences();
		openScreen("sequencer");
		break;
	}
}
