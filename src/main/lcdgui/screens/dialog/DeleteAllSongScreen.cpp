#include "DeleteAllSongScreen.hpp"
#include "sequencer/Sequencer.hpp"

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
            openScreenById(ScreenId::DeleteSongScreen);
            break;
        case 4:

            for (int j = 0; j < 20; j++)
            {
                sequencer->deleteSong(j);
            }

            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}
