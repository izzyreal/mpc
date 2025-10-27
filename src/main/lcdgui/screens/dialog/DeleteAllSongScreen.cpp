#include "DeleteAllSongScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSongScreen::DeleteAllSongScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-all-song", layerIndex)
{
}

void DeleteAllSongScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->openScreen<DeleteSongScreen>();
            break;
        case 4:

            for (int j = 0; j < 20; j++)
            {
                sequencer->deleteSong(j);
            }

            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}
