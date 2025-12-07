#include "DeleteAllTracksScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllTracksScreen::DeleteAllTracksScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-all-tracks", layerIndex)
{
}

void DeleteAllTracksScreen::function(const int i)
{
    if (i == 3)
    {
        openScreenById(ScreenId::DeleteTrackScreen);
    }
    else if (i == 4)
    {
        sequencer.lock()->copySelectedSequenceToUndoSequence();
        sequencer.lock()->getSelectedSequence()->deleteAllTracks();
        openScreenById(ScreenId::SequencerScreen);
    }
}
