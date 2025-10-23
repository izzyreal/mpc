#include "DeleteAllSequencesScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSequencesScreen::DeleteAllSequencesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-sequences", layerIndex)
{
}

void DeleteAllSequencesScreen::function(int i)
{
	
	switch (i)
	{
	case 3:
        mpc.getLayeredScreen()->openScreen<DeleteSequenceScreen>();
		break;
	case 4:
		sequencer.lock()->move(0);
		sequencer.lock()->purgeAllSequences();
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
		break;
	}
}
