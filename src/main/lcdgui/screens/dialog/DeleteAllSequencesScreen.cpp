#include "DeleteAllSequencesScreen.hpp"

#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllSequencesScreen::DeleteAllSequencesScreen(const int layerIndex)
	: ScreenComponent("delete-all-sequences", layerIndex)
{
}

void DeleteAllSequencesScreen::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("delete-sequence");
		break;
	case 4:
		sequencer.lock()->purgeAllSequences();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
