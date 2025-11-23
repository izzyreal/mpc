#include "DeleteSongScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

#include "StrUtil.hpp"

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
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "song")
    {
        const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
        auto candidate = songScreen->getSelectedSongIndex() + i;

        if (candidate < 0)
        {
            candidate = 0;
        }
        if (candidate > 19)
        {
            candidate = 19;
        }

        songScreen->setSelectedSongIndex(candidate);

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
        {
            const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
            sequencer.lock()->deleteSong(songScreen->getSelectedSongIndex());
            openScreenById(ScreenId::SongScreen);
            break;
        }
        default:;
    }
}

void DeleteSongScreen::displaySong() const
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song =
        sequencer.lock()->getSong(songScreen->getSelectedSongIndex());
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(songScreen->getSelectedSongIndex() + 1),
                         "0", 2) +
        "-" + song->getName());
}
