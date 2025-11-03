#include "DeleteAllTracksScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllTracksScreen::DeleteAllTracksScreen(mpc::Mpc &mpc,
                                             const int layerIndex)
    : ScreenComponent(mpc, "delete-all-tracks", layerIndex)
{
}

void DeleteAllTracksScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::DeleteTrackScreen);
            break;
        case 4:
            sequencer->getActiveSequence()->purgeAllTracks();
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}
