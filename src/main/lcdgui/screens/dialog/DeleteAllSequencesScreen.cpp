#include "DeleteAllSequencesScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSequencesScreen::DeleteAllSequencesScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "delete-all-sequences", layerIndex)
{
}

void DeleteAllSequencesScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::DeleteSequenceScreen);
            break;
        case 4:
            sequencer->move(0);
            sequencer->purgeAllSequences();
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}
