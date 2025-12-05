#include "DeleteSongScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteSongScreen::DeleteSongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-song", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSongIndex();
                        },
                        [&](auto)
                        {
                            displaySong();
                        }});
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
        const auto lockedSequencer = sequencer.lock();

        lockedSequencer->setSelectedSongIndex(
            lockedSequencer->getSelectedSongIndex() + i);

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
            const auto lockedSequencer = sequencer.lock();

            lockedSequencer->deleteSong(
                lockedSequencer->getSelectedSongIndex());

            openScreenById(ScreenId::SongScreen);
            break;
        }
        default:;
    }
}

void DeleteSongScreen::displaySong() const
{
    const auto lockedSequencer = sequencer.lock();

    const auto song = lockedSequencer->getSelectedSong();

    findField("song")->setText(
        StrUtil::padLeft(
            std::to_string(lockedSequencer->getSelectedSongIndex() + 1), "0",
            2) +
        "-" + song->getName());
}
