#include "CopySongScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"
#include "lcdgui/screens/SongScreen.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopySongScreen::CopySongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-song", layerIndex)
{
}

void CopySongScreen::open()
{
    song1 = 0;

    for (int i = 0; i < 20; i++)
    {
        if (!sequencer.lock()->getSong(i)->isUsed())
        {
            song1 = i;
            break;
        }
    }

    displaySong0();
    displaySong1();
}

void CopySongScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SongWindow);
            break;
        case 4:
        {
            const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
            sequencer.lock()->copySong(songScreen->getSelectedSongIndex(), song1);
            openScreenById(ScreenId::SongScreen);
            break;
        }
        default:;
    }
}

void CopySongScreen::turnWheel(const int i)
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "song0")
    {
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

        displaySong0();
    }
    else if (focusedFieldName == "song1")
    {
        setSong1(song1 + i);
    }
}

void CopySongScreen::setSong1(const int i)
{
    song1 = std::clamp(i, 0, 19);
    displaySong1();
}

void CopySongScreen::displaySong0() const
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song = sequencer.lock()->getSong(songScreen->getSelectedSongIndex());
    findField("song0")->setText(
        StrUtil::padLeft(std::to_string(songScreen->getSelectedSongIndex() + 1),
                         "0", 2) +
        "-" + song->getName());
}

void CopySongScreen::displaySong1() const
{
    const auto song = sequencer.lock()->getSong(song1);
    findField("song1")->setText(
        StrUtil::padLeft(std::to_string(song1 + 1), "0", 2) + "-" +
        song->getName());
}
