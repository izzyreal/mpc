#include "CopySongScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopySongScreen::CopySongScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-song", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSongIndex();
                        },
                        [&](auto)
                        {
                            displaySong0();
                        }});
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
            const auto lockedSequencer = sequencer.lock();
            lockedSequencer->copySong(lockedSequencer->getSelectedSongIndex(),
                                      song1);
            openScreenById(ScreenId::SongScreen);
            break;
        }
        default:;
    }
}

void CopySongScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "song0")
    {
        const auto lockedSequencer = sequencer.lock();
        lockedSequencer->setSelectedSongIndex(
            lockedSequencer->getSelectedSongIndex() + i);
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
    const auto lockedSequencer = sequencer.lock();
    const auto song = lockedSequencer->getSelectedSong();

    findField("song0")->setText(
        StrUtil::padLeft(
            std::to_string(lockedSequencer->getSelectedSongIndex() + 1), "0",
            2) +
        "-" + song->getName());
}

void CopySongScreen::displaySong1() const
{
    const auto song = sequencer.lock()->getSong(song1);
    findField("song1")->setText(
        StrUtil::padLeft(std::to_string(song1 + 1), "0", 2) + "-" +
        song->getName());
}
