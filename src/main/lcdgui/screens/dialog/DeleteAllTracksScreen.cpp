#include "DeleteAllTracksScreen.hpp"

#include "sequencer/Sequence.hpp"

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
            mpc.getLayeredScreen()->openScreen<DeleteTrackScreen>();
            break;
        case 4:
            sequencer->getActiveSequence()->purgeAllTracks();
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}
