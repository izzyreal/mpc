#include "DeleteSongScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteSongScreen::DeleteSongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-song", layerIndex)
{
}

void DeleteSongScreen::open()
{
    displaySong();
}

void DeleteSongScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "song")
    {
        const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
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

void DeleteSongScreen::function(const int i)
{

    switch (i)
    {
        case 2:
            openScreenById(ScreenId::DeleteAllSongScreen);
            break;
        case 3:
            openScreenById(ScreenId::SongWindow);
            break;
        case 4:
            const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
            sequencer->deleteSong(songScreen->activeSongIndex);
            openScreenById(ScreenId::SongScreen);
            break;
    }
}

void DeleteSongScreen::displaySong()
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song = sequencer->getSong(songScreen->activeSongIndex);
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(songScreen->activeSongIndex + 1), "0",
                         2) +
        "-" + song->getName());
}
