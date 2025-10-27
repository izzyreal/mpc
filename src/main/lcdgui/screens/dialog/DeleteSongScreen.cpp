#include "DeleteSongScreen.hpp"

#include "lcdgui/screens/SongScreen.hpp"
#include "sequencer/Song.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteSongScreen::DeleteSongScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-song", layerIndex)
{
}

void DeleteSongScreen::open()
{
    displaySong();
}

void DeleteSongScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "song")
    {
        auto songScreen = mpc.screens->get<SongScreen>();
        auto candidate = songScreen->activeSongIndex + i;

        if (candidate < 0)
        {
            candidate = 0;
        }
        if (candidate > 19)
        {
            candidate = 19;
        }

        songScreen->activeSongIndex = candidate;

        displaySong();
    }
}

void DeleteSongScreen::function(int i)
{

    switch (i)
    {
        case 2:
            mpc.getLayeredScreen()->openScreen<DeleteAllSongScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<SongWindow>();
            break;
        case 4:
            auto songScreen = mpc.screens->get<SongScreen>();
            sequencer->deleteSong(songScreen->activeSongIndex);
            mpc.getLayeredScreen()->openScreen<SongScreen>();
            break;
    }
}

void DeleteSongScreen::displaySong()
{
    auto songScreen = mpc.screens->get<SongScreen>();
    auto song = sequencer->getSong(songScreen->activeSongIndex);
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(songScreen->activeSongIndex + 1), "0",
                         2) +
        "-" + song->getName());
}
